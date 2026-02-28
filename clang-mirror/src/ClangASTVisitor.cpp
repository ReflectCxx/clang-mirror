
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


    std::optional<std::string> ClangASTVisitor::getHashIncludeStr(const TagDecl* pTypeDecl, std::string_view pTypeStr,
                                                                  bool pShouldBePublic)
    {
        if (pTypeDecl)
        {
            auto& SM = pTypeDecl->getASTContext().getSourceManager();
            SourceLocation loc = SM.getExpansionLoc(pTypeDecl->getLocation());
            const FileEntry* file = SM.getFileEntryForID(SM.getFileID(loc));

            bool skipFile = pShouldBePublic;
            if (pShouldBePublic) {
                skipFile = isPublicHeader(file);
            }
            if (!skipFile) {
                return m_preProcessor.getIncludeStrAsWritten(file, pTypeStr);
            }
        }
        return std::optional<std::string>();
    }


    bool ClangASTVisitor::isHeaderReachableForType(const QualType& pQT,
                                                   const FunctionDecl* pFnDecl,
                                                   const std::string& pTypeStr,
                                                   const FileEntry* pSrcHeader)
    {
        QualType QT = pQT.getNonReferenceType().getUnqualifiedType();
        if (QT->isPointerType()) {
            QT = QT->getPointeeType();
        }

        if (QT->isIncompleteType()) {
            Logger::outDbg("incomplete type: " + pTypeStr);
            return false;
        }

        if (auto incf = ASTDeclsUtils::resolveHeaderFromType(QT, pFnDecl->getASTContext(), m_preProcessor)) {
            //if (!m_preProcessor.isFileReachableFromHeader(pSrcHeader, incf)) {
            //    Logger::outDbg("header not reachable for type: " + pTypeStr);
            //    return false;
            //}
        }
        else {
            Logger::outDbg("(err) header not found for type: " + pTypeStr);
            return false;
        }
        return true;
    }


    bool ClangASTVisitor::extractArgsAndItsHeaders(const FunctionDecl* pFnDecl,
                                                   const FileEntry* pDeclFile,
                                                   std::vector<std::string>& pArgsStrs,
                                                   std::vector<std::string>& pHeaders)
    {
        const auto& params = pFnDecl->parameters();
        const auto& fnQName = pFnDecl->getQualifiedNameAsString();
        auto& SM = pFnDecl->getASTContext().getSourceManager();

        for (unsigned index = 0; index < params.size(); index++)
        {
            auto qT = params[index]->getOriginalType();
            const auto& argStr = ASTDeclsUtils::extractQualifiedTypeName(qT);
            pArgsStrs.push_back(argStr);

            qT = qT.getNonReferenceType().getUnqualifiedType();
            if (qT->isPointerType()) {
                qT = qT->getPointeeType();
            }

            if (qT->isBuiltinType()) {
                continue;
            }

            if ( taggedForExclusion(argStr) ||
                !isHeaderReachableForType(qT, pFnDecl, argStr, pDeclFile)) {
                return false;
            }

            auto* T = qT.getTypePtrOrNull();
            if (const RecordType* RT = T->getAs<RecordType>()) {
                const CXXRecordDecl* RD = llvm::dyn_cast<CXXRecordDecl>(RT->getDecl());
                if (RD) {
                    auto incStr = getHashIncludeStr(RD->getDefinition(), argStr, false);
                    if (incStr) {
                        pHeaders.push_back(*incStr);
                    }
                }
            }
            else if (const EnumType* ET = T->getAs<EnumType>()) {
                const EnumDecl* ED = ET->getDecl();
                if (ED) {
                    if (const EnumDecl* Def = ED->getDefinition()) {
                        auto incStr = getHashIncludeStr(Def, argStr, false);
                        if (incStr) {
                            pHeaders.push_back(*incStr);
                        }
                    }
                    else Logger::outDbg("(ast-err) unresolved arg-type: " + argStr);
                }
                else Logger::outDbg("(ast-err) unresolved arg-type: " + argStr);
            }
            else {
                Logger::outDbg("(err) unresolved arg-type: " + argStr);
                return false;
            }
        }
        return true;
    }


    std::optional<std::string> ClangASTVisitor::getReturnStrAndItsHeaders(const FunctionDecl* pFnDecl,
                                                                          const FileEntry* pDeclFile,
                                                                          std::vector<std::string>& pHeaders)
    {
        const auto returnStr = ASTDeclsUtils::extractQualifiedTypeName(pFnDecl->getReturnType());
        if (taggedForExclusion(returnStr)) {
            return std::nullopt;
        }
        auto qT = pFnDecl->getReturnType().getNonReferenceType();
        if (qT->isPointerType()) {
            qT = qT->getPointeeType();
        }

        if (!qT->isBuiltinType()) {
            if (!isHeaderReachableForType(qT, pFnDecl, returnStr, pDeclFile)) {
                return std::nullopt;
            }
            auto incStr = getHashIncludeStr(qT->getAsTagDecl()->getDefinition(), returnStr, false);
            if (incStr) {
                pHeaders.push_back(*incStr);
            }
        }
        return returnStr;
    }


    bool ClangASTVisitor::VisitFunctionDecl(FunctionDecl* pFnDecl)
    {
        if ( pFnDecl->getDefinition() != pFnDecl ||
             pFnDecl->isDeleted() ||
             pFnDecl->isImplicit() ||
             pFnDecl->isOverloadedOperator() ||
             pFnDecl->isFunctionTemplateSpecialization() ||
             pFnDecl->getLinkageInternal() != Linkage::External ||
             isa<CXXConversionDecl>(pFnDecl) || isa<CXXDestructorDecl>(pFnDecl) ||
            (isa<CXXMethodDecl>(pFnDecl) && cast<CXXMethodDecl>(pFnDecl)->getAccess() != AS_public)) {
            return true;
        }

        auto& SM = pFnDecl->getASTContext().getSourceManager();
        SourceLocation loc = SM.getExpansionLoc(pFnDecl->getLocation());
        if (loc.isInvalid() || !SM.isInMainFile(loc)) {
            return true;
        }

        auto* ctor = llvm::dyn_cast<CXXConstructorDecl>(pFnDecl);
        if (ctor && ctor->getNumParams() == 0) {
            return true;
        }

        auto* method = llvm::dyn_cast<CXXMethodDecl>(pFnDecl);
        if (method) {
            if(method->isOverloadedOperator()) {
                return true;
            }
            auto* record = method->getParent();
            if (record->getAccess() == AS_private || record->getAccess() == AS_protected) {
                return true;
            }
        }

        Logger::outDbg(pFnDecl->getNameAsString() + "()", "^^");

        auto* declFile = getDeclaringFile(pFnDecl);
        if (declFile && isPublicHeader(declFile)) {
            addReflectableEntity(pFnDecl, declFile);
        }
        else {
            Logger::outDbg("(skip)");
        }
        return true;
    }


    void ClangASTVisitor::addReflectableEntity(const FunctionDecl* pFnDecl, const FileEntry* pDeclFile)
    {
        auto [metaKind, fname] = ASTDeclsUtils::getNameAndMetaKind(pFnDecl);
        if (metaKind == MetaKind::None || taggedForExclusion(fname)) {
            Logger::outDbg("(skip)");
            return;
        }

        const std::string recordStr = ASTDeclsUtils::extractParentTypeName(pFnDecl);
        if (taggedForExclusion(recordStr)) {
            Logger::outDbg("(skip)");
            return;
        }

        std::vector<std::string> headers;
        std::vector<std::string> argsTypeStr;
        if(!extractArgsAndItsHeaders(pFnDecl, pDeclFile, argsTypeStr, headers)) {
            Logger::outDbg("(skip)");
            return;
        }
        
        auto returnStr = getReturnStrAndItsHeaders(pFnDecl, pDeclFile, headers);
        if (!returnStr) {
            Logger::outDbg("(skip)");
            return;
        }

        auto hashIncludeStr = m_preProcessor.getIncludeStrAsWritten(pDeclFile, fname + "()");
        if (!hashIncludeStr) {
            Logger::outDbg("(skip)");
            return;
        }
        headers.push_back(*hashIncludeStr);
        
        auto* codeBuffer = ASTCodeManager::instance().getCodeBuffer(m_srcFile, true);
        codeBuffer->addFunction(metaKind, {
                .headers = headers,
                .function = fname
        }, recordStr, *returnStr, StringUtils::getParamTypesStr(argsTypeStr));
    }
}