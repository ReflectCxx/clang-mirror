
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


    std::optional<std::string> ClangASTVisitor::getHashIncludeStr(const TagDecl* pTypeDecl)
    {
        if (pTypeDecl)
        {
            auto& SM = pTypeDecl->getASTContext().getSourceManager();
            SourceLocation loc = SM.getExpansionLoc(pTypeDecl->getLocation());
            const FileEntry* file = SM.getFileEntryForID(SM.getFileID(loc)); 
            return m_preProcessor.getHashIncludeAsWritten(file);
        }
        return std::optional<std::string>();
    }


    RegErr ClangASTVisitor::isHeaderReachableForType(const QualType& pQT,
                                                     const ASTContext& pCtx,
                                                     const FileEntry* pMainHeader)
    {
        auto qT = desugarQT(pQT, pCtx);
        if (qT->isIncompleteType()) {
            return RegErr::IncompleteType;
        }

        auto [err, incf] = ASTDeclsUtils::resolveHeaderFromType(pQT, pCtx, m_preProcessor);
        if (!incf) {
            return err;
        }
        if (!m_preProcessor.isHeaderReachableFromSrc(pMainHeader, incf)) {
           return RegErr::AstParsing;
        }
        return RegErr::None;
    }


    RegErr ClangASTVisitor::extractArgsAndItsHeaders(const FunctionDecl* pFnDecl,
                                                     const FileEntry* pHeaderFile,
                                                     std::vector<std::string>& pArgsStrs,
                                                     std::vector<std::string>& pHeaders)
    {
        const auto& fnQName = pFnDecl->getQualifiedNameAsString();
        auto& SM = pFnDecl->getASTContext().getSourceManager();

        for (auto* argDecl : pFnDecl->parameters())
        {
            const auto& qT = argDecl->getType();
            auto err = getTypeDefiningHeader(qT, pFnDecl->getASTContext(), pHeaderFile, pHeaders);
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


    RegErr ClangASTVisitor::getTypeDefiningHeader(const clang::QualType& pQT,
                                                  const clang::ASTContext& pCtx,
                                                  const FileEntry* pMainHeader,
                                                  std::vector<std::string>& pHeaders)
    {
        auto qT = desugarQT(pQT, pCtx);
        if (qT->isBuiltinType()) {
            return RegErr::None;
        }
        
        auto err = isHeaderReachableForType(pQT, pCtx, pMainHeader);
        if (err != RegErr::None) {
            return err;
        }

        auto* T = qT.getTypePtrOrNull();
        if (const RecordType* RT = T->getAs<RecordType>()) {
            const CXXRecordDecl* RD = llvm::dyn_cast<CXXRecordDecl>(RT->getDecl());
            if (RD) {
                auto incStr = getHashIncludeStr(RD->getDefinition());
                if (incStr) {
                    pHeaders.push_back(*incStr);
                    return RegErr::None;
                }
            }
        }
        else if (const EnumType* ET = T->getAs<EnumType>()) {
            const EnumDecl* ED = ET->getDecl();
            if (ED) {
                if (const EnumDecl* Def = ED->getDefinition()) {
                    auto incStr = getHashIncludeStr(Def);
                    if (incStr) {
                        pHeaders.push_back(*incStr);
                        return RegErr::None;
                    }
                }
            }
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

        err = getTypeDefiningHeader(pFnDecl->getReturnType(), pFnDecl->getASTContext(), pHeaderFile, headers);
        if (err != RegErr::None) {
            return err;
        }

        const auto returnStr = ASTDeclsUtils::extractQualifiedTypeName(pFnDecl->getReturnType());
        err = taggedForExclusion(returnStr);
        if (err != RegErr::None) {
            return err;
        }
        
        std::vector<std::string> argsTypeStr;
        err = extractArgsAndItsHeaders(pFnDecl, pHeaderFile, argsTypeStr, headers);
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

        auto* headerFile = getDeclaringFile(pFnDecl);
        if (headerFile) {
            auto err = RegErr::HeaderNotPublic;
            if (isPublicHeader(headerFile)) {
                err = addReflectableEntity(pFnDecl, headerFile);
            }
            if (err != RegErr::None && err != RegErr::ExclusionByPolicy && err != RegErr::HeaderNotPublic) {
                Logger::outDbg(pFnDecl->getNameAsString() + "()", err, "^^");
            }
        }
        else {
            Logger::outDbg(pFnDecl->getNameAsString() + "()", RegErr::AstParsing, "^^");
        }
        return true;
    }
}