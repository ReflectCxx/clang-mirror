
#include <iostream>
#include <algorithm>

#include "Constants.h"
#include "StringUtils.h"
#include "ASTCodeManager.h"
#include "ASTCodeBuffer.h"
#include "ASTDeclsUtils.h"
#include "ClangPPCallbacks.h"

#include "ClangASTVisitor.h"
#include "ClangASTVisitor.hpp"

#include "clang/AST/Type.h"
#include "clang/AST/PrettyPrinter.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

namespace clmr
{
    ClangASTVisitor::ClangASTVisitor(const std::string& pSrcFile, ClangPPCallbacks& pPP)
        : m_srcFile(pSrcFile)
        , m_preProcessor(pPP)
    { }

    RegErr ClangASTVisitor::extractArgsAndItsHeaders(const FunctionDecl* pFnDecl,
                                                     std::vector<std::string>& pArgsStrs,
                                                     std::vector<std::string>& pHeaders)
    {
        const auto& fnQName = pFnDecl->getQualifiedNameAsString();
        auto& SM = pFnDecl->getASTContext().getSourceManager();

        for (auto* argDecl : pFnDecl->parameters())
        {
            const auto& qT = argDecl->getType();
            auto err = addTypeDefiningHeader(pHeaders, qT, pFnDecl->getASTContext());
            if (err != RegErr::None) {
                return err;
            }

            const auto& argStr = ASTDeclsUtils::extractQualifiedTypeName(qT);
            err = taggedForExclusion(argStr);
            if (err != RegErr::None) {
                return err;
            }
            pArgsStrs.push_back(argStr);
        }
        return RegErr::None;
    }


    RegErr ClangASTVisitor::addTypeDefiningHeader(std::vector<std::string>& pHeaders,
                                                  const clang::QualType& pQT,
                                                  const clang::ASTContext& pCtx)
    {
        auto qT = desugarQT(pQT, pCtx);
        if (qT->isBuiltinType()) {
            return RegErr::None;
        }
        
        if (qT->isIncompleteType()) {
            return RegErr::IncompleteType;
        }

        auto [err, incFile] = ASTDeclsUtils::getHeaderDefiningType(pQT, pCtx, m_preProcessor);
        if (err != RegErr::None) {
            return err;
        }

        incFile = m_preProcessor.getFileDoingHashIncludeFor(incFile);
        if (!incFile) {
            return RegErr::AstParsing;
        }

        auto incStr = m_preProcessor.getHashIncludeAsWritten(incFile);
        if (incStr) {
            pHeaders.push_back(*incStr);
            return RegErr::None;
        }
        return RegErr::AstParsing;
    }


    RegErr ClangASTVisitor::addReflectableEntity(const FunctionDecl* pFnDecl, const FileEntry* pHeaderFile)
    {
        std::vector<std::string> headers;
        auto hashIncludeStr = m_preProcessor.getHashIncludeAsWritten(pHeaderFile);
        if (!hashIncludeStr) {
            return RegErr::AstParsing;
        }
        headers.push_back(*hashIncludeStr);

        auto [metaKind, fname] = ASTDeclsUtils::getNameAndMetaKind(pFnDecl);
        if (metaKind == MetaKind::None) {
            return RegErr::AstParsing;
        }

        auto err = taggedForExclusion(fname);
        if (err != RegErr::None) {
            return err;
        }

        auto recordStr = ASTDeclsUtils::extractParentTypeName(pFnDecl);
        err = taggedForExclusion(recordStr);
        if (err != RegErr::None) {
            return err;
        }

        const auto returnStr = ASTDeclsUtils::extractQualifiedTypeName(pFnDecl->getReturnType());
        err = taggedForExclusion(returnStr);
        if (err != RegErr::None) {
            return err;
        }

        err = addTypeDefiningHeader(headers, pFnDecl->getReturnType(), pFnDecl->getASTContext());
        if (err != RegErr::None) {
            return err;
        }
        
        std::vector<std::string> argsTypeStr;
        err = extractArgsAndItsHeaders(pFnDecl, argsTypeStr, headers);
        if (err != RegErr::None) {
            return err;
        }

        auto* codeBuffer = ASTCodeManager::instance().getCodeBuffer(m_srcFile, true);
        codeBuffer->addFunction(metaKind, {
            .headers = headers,
            .function = fname
        }, recordStr, returnStr, StringUtils::getParamTypesStr(argsTypeStr));
        return RegErr::None;
    }


    bool ClangASTVisitor::VisitFunctionDecl(FunctionDecl* pFnDecl)
    {
        if (!isReflectableEntity(pFnDecl)) {
            return true;
        }

        auto fnStr = pFnDecl->getNameAsString() + "()";
        auto* headerFile = getDeclaringFile(pFnDecl);

        if (!headerFile) {
            Logger::outDbg(pFnDecl->getNameAsString() + "()", RegErr::HeaderNotFound);
            return true;
        }

        auto err = RegErr::HeaderNotPublic;
        if (isPublicHeader(headerFile))
        {
            err = addReflectableEntity(pFnDecl, headerFile);
            if (err == RegErr::None) {
                return true;
            }
        }

        Logger::outDbg(fnStr, err);
        return true;
    }
}