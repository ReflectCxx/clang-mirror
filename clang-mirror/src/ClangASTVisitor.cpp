
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
            const auto& argStr = ASTDeclsUtils::extractQualifiedTypeName(qT);
            auto err = filterByExclusion(argStr);
            if (err != RegErr::None) {
                return err;
            }

            err = addTypeDefiningHeader(qT, pFnDecl->getASTContext(), pHeaders);
            if (err != RegErr::None) {
                Logger::outDbg("error processing arg-type : " + argStr);
                return err;
            }
            pArgsStrs.push_back(argStr);
        }
        return RegErr::None;
    }


    RegErr ClangASTVisitor::addTypeDefiningHeader(const clang::QualType& pQT,
                                                  const clang::ASTContext& pCtx,
                                                  std::vector<std::string>& pHeaders)
    {
        auto qT = desugarQT(pQT, pCtx);
        if (qT->isBuiltinType()) {
            return RegErr::None;
        }
        
        if (qT->isIncompleteType()) {
            return RegErr::IncompleteType;
        }

        auto [err0, incFile] = ASTDeclsUtils::getHeaderDefiningType(pQT, pCtx);
        if (err0 != RegErr::None) {
            return err0;
        }

        auto [err1, incStr] = m_preProcessor.getHashIncludeAsWrittenFor(incFile);
        if (err1!= RegErr::None) {
            return err1;
        }

        pHeaders.push_back(incStr);
        return RegErr::None;
    }


    RegErr ClangASTVisitor::addReflectableEntity(const FunctionDecl* pFnDecl)
    {
        std::vector<std::string> headers;
        auto headerFile = getDeclaringFile(pFnDecl);

        if (headerFile) {
            auto [err, hashIncStr] = m_preProcessor.getHashIncludeAsWrittenFor(headerFile);
            if (err != RegErr::None) {
                Logger::outDbg("error while processing function : " + pFnDecl->getNameAsString());
                return err;
            }
            headers.push_back(hashIncStr);
        }

        auto [metaKind, fname] = ASTDeclsUtils::getNameAndMetaKind(pFnDecl);
        if (metaKind == MetaKind::None) {
            Logger::outDbg("failed to classify as MetaKind : " + pFnDecl->getNameAsString());
            return RegErr::AstParsing;
        }

        auto err = filterByExclusion(fname);
        if (err != RegErr::None) {
            return err;
        }

        auto recordStr = ASTDeclsUtils::extractParentTypeName(pFnDecl);
        err = filterByExclusion(recordStr);
        if (err != RegErr::None) {
            return err;
        }

        const auto returnStr = ASTDeclsUtils::extractQualifiedTypeName(pFnDecl->getReturnType());
        err = filterByExclusion(returnStr);
        if (err != RegErr::None) {
            return err;
        }

        err = addTypeDefiningHeader(pFnDecl->getReturnType(), pFnDecl->getASTContext(), headers);
        if (err != RegErr::None) {
            Logger::outDbg("error processing return-type : " + returnStr);
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

        auto err = addReflectableEntity(pFnDecl);
        if (err == RegErr::None) {
            return true;
        }

        auto fnStr = pFnDecl->getNameAsString() + "()";
        Logger::outDbg(fnStr, err);
        return true;
    }
}