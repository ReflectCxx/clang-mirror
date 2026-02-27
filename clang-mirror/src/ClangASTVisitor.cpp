
#include <iostream>
#include <algorithm>
#include <filesystem>

#include "Logger.h"
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


    std::optional<std::string> ClangASTVisitor::getHashIncludeStr(clang::Decl* pTypeDecl, std::string_view pTypeStr,
                                                                  bool pShouldBePublic)
    {
        if (pTypeDecl)
        {
            auto& SM = pTypeDecl->getASTContext().getSourceManager();
            SourceLocation loc = SM.getExpansionLoc(pTypeDecl->getLocation());
            const FileEntry* file = SM.getFileEntryForID(SM.getFileID(loc));

            bool skipFile = pShouldBePublic;
            if(pShouldBePublic) {
                skipFile = isPublicHeader(file);
            }
            if (!skipFile) {
                return m_preProcessor.getIncludeStrAsWritten(file, pTypeStr);
            }
        }
        return std::optional<std::string>();
    }


    bool ClangASTVisitor::isHeaderReachableForType(const clang::QualType& pQT,
                                                   const clang::FunctionDecl *pFnDecl,
                                                   const std::string& pTypeStr,
                                                   const clang::FileEntry* pSrcHeader)
    {
        QualType QT = pQT.getNonReferenceType().getUnqualifiedType();
        if (QT->isPointerType()) {
            QT = QT->getPointeeType();
        }

        if (QT->isIncompleteType()) {
            Logger::outDbg("[skip] (incomplete type) " + pTypeStr);
            return false;
        }

        if (auto incf = ASTDeclsUtils::resolveHeaderFromType(QT, pFnDecl->getASTContext(), m_preProcessor)) {
            //if (!m_preProcessor.isFileReachableFromHeader(pSrcHeader, incf)) {
            //    Logger::outDbg("header not reachable for type: " + pTypeStr);
            //    return false;
            //}
        }
        else {
            Logger::outDbg("header not found for type: " + pTypeStr);
            return false;
        }
        return true;
    }


    bool ClangASTVisitor::VisitFunctionDecl(FunctionDecl* pFnDecl)
    {
        if ( pFnDecl->isImplicit() ||
             pFnDecl->isDeleted() ||
             pFnDecl->isInAnonymousNamespace() ||
             pFnDecl->isFunctionTemplateSpecialization() ||
            (pFnDecl->isGlobal() && pFnDecl->isStatic()) ||
             pFnDecl->isOverloadedOperator() ||
             pFnDecl->getKind() == Decl::Kind::CXXDestructor ||
             pFnDecl->getKind() == Decl::Kind::CXXConversion ||
             pFnDecl->getAccess() == AS_private ||
             pFnDecl->getAccess() == AS_protected ||
             pFnDecl->getLinkageInternal() != Linkage::External) {
            return true;
        }

        if (!pFnDecl->isThisDeclarationADefinition()) {
            return true;
        }

        auto& SM = pFnDecl->getASTContext().getSourceManager();
        SourceLocation loc = SM.getExpansionLoc(pFnDecl->getLocation());
        if (loc.isInvalid() || !SM.isInMainFile(loc)) {
            return true;
        }

        const auto* method = llvm::dyn_cast<CXXMethodDecl>(pFnDecl);
        if (method) {
            if(method->isOverloadedOperator() || llvm::isa<clang::CXXConversionDecl>(method)) {
                return true;
            }
            const CXXRecordDecl* record = method->getParent();
            if (record->getAccess() != AS_public) {
                return true;
            }
        }

        auto* declFile = getDeclaringFile(pFnDecl);
        if (declFile && isPublicHeader(declFile)) {
            addReflectableEntity(pFnDecl, declFile);
        }
        return true;
    }


    void ClangASTVisitor::addReflectableEntity(const FunctionDecl* pFnDecl, const FileEntry* pDeclFile)
    {
        std::vector<std::string> headers;
        std::vector<std::string> parmTypes;
        const auto& params = pFnDecl->parameters();
        const auto& fnQName = pFnDecl->getQualifiedNameAsString();
        auto& SM = pFnDecl->getASTContext().getSourceManager();

        for (unsigned index = 0; index < params.size(); index++)
        {
            const auto& qT = params[index]->getOriginalType();
            const auto& argStr = ASTDeclsUtils::extractQualifiedTypeName(qT);
            if (qT->isBuiltinType() || taggedForExclusion(argStr)) {
                return;
            }

            if (!isHeaderReachableForType(qT, pFnDecl, argStr, pDeclFile)) {
                return;
            }
            parmTypes.push_back(argStr);

            auto* T = params[index]->getOriginalType().getTypePtrOrNull();
            if (const RecordType* RT = T->getAs<RecordType>()) 
            {
                const CXXRecordDecl* RD = llvm::dyn_cast<CXXRecordDecl>(RT->getDecl());
                if (RD) {
                    auto incStr = getHashIncludeStr(RD->getDefinition(), argStr, false);
                    if (incStr) {
                        headers.push_back(*incStr);
                    }
                }
            }
            else {
                Logger::outDbg("[skip] arg-type header lookup for : " + argStr);
            }
        }

        const auto returnStr = ASTDeclsUtils::extractQualifiedTypeName(pFnDecl->getReturnType());
        if (taggedForExclusion(returnStr)) {
            return;
        }

        auto qT = pFnDecl->getReturnType().getNonReferenceType();
        if (qT->isPointerType()) {
            qT = qT->getPointeeType();
        }

        if (!qT->isBuiltinType()) {
            if (!isHeaderReachableForType(qT, pFnDecl, returnStr, pDeclFile)) {
                return;
            }
            auto incStr = getHashIncludeStr(qT->getAsTagDecl()->getDefinition(), returnStr, false);
            if (incStr) {
                headers.push_back(*incStr);
            }
        }
        
        auto [metaKind, fname] = ASTDeclsUtils::getNameAndMetaKind(pFnDecl);
        if (metaKind == MetaKind::None) return;

        const std::string recordStr = ASTDeclsUtils::extractParentTypeName(pFnDecl);
        if (taggedForExclusion(fname) || 
            taggedForExclusion(returnStr) || 
            taggedForExclusion(recordStr)) {
            return;
        }

        auto hashIncludeStr = m_preProcessor.getIncludeStrAsWritten(pDeclFile, fname + "()");
        if (!hashIncludeStr) {
            return;
        }

        headers.push_back(*hashIncludeStr);
        auto* codeBuffer = ASTCodeManager::instance().getCodeBuffer(m_srcFile, true);
        codeBuffer->addFunction(metaKind, {
                .headers = headers,
                .function = fname
        }, recordStr, returnStr, StringUtils::getParamTypesStr(parmTypes));
    }
}