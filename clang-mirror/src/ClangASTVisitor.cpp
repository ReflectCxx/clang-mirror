
#include <iostream>
#include <algorithm>

#include "Logger.h"
#include "Constants.h"
#include "StringUtils.h"
#include "ASTCodeManager.h"
#include "ASTCodeBuffer.h"
#include "ASTDeclsUtils.h"
#include "ClangASTVisitor.h"
#include "ClangPPCallbacks.h"

#include "clang/AST/Type.h"
#include "clang/AST/PrettyPrinter.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

namespace {

    bool shouldBeExcluded(const std::string& pStr)
    {
        const auto& exclusions = clmr::ASTCodeManager::instance().getExcludeNamespaces();
        for (const auto& excStr : exclusions) {
            if (pStr.find(excStr + "::") != std::string::npos ||
                // exclude templates as well. (not supporting yet)
                pStr.find('<') != std::string::npos) {
                return true;
            }
        }
        return false;
    }
}

namespace clmr
{
    ClangASTVisitor::ClangASTVisitor(const std::string& pSrcFile, ClangPPCallbacks& pPP)
        : m_srcFile(pSrcFile)
        , m_preProcessor(pPP)
	{ }

    bool ClangASTVisitor::VisitFunctionDecl(FunctionDecl* pFnDecl)
    {
        if (!ASTDeclsUtils::isInUserCode(pFnDecl) ||
            pFnDecl->isDeleted() ||
            pFnDecl->isInAnonymousNamespace() ||
            (pFnDecl->isGlobal() && pFnDecl->isStatic()) ||
            pFnDecl->isOverloadedOperator() ||
            pFnDecl->getKind() == Decl::Kind::CXXDestructor ||
            pFnDecl->getAccess() == AS_private ||
            pFnDecl->getAccess() == AS_protected ||
            pFnDecl->getLinkageInternal() != Linkage::External) {
            return true;
        }

        if (!pFnDecl->isThisDeclarationADefinition()) {
            return true;
        }

        if (pFnDecl->getFirstDecl() == nullptr) {
            return true;
        }

        auto& SM = pFnDecl->getASTContext().getSourceManager();
        for (auto* decl : pFnDecl->redecls()) {
            SourceLocation loc = SM.getSpellingLoc(decl->getLocation());
            if (SM.isInSystemHeader(loc)) {
                return true;
            }
        }

        if (!ASTDeclsUtils::isDeclFrmCurrentSource(m_srcFile, pFnDecl)) {
            return true;
        }

        const auto* method = llvm::dyn_cast<clang::CXXMethodDecl>(pFnDecl);
        if (method) {
            const clang::CXXRecordDecl* record = method->getParent();
            if (record->getAccess() != AS_public) {
                return true;
            }
        }

        //auto& srcMgr = pFnDecl->getASTContext().getSourceManager();
        //auto headerStr = ASTDeclsUtils::resolveHeaderFromDecl(pFnDecl, srcMgr, m_preProcessor);
        //if (headerStr) {
            addReflectableEntity(pFnDecl, "");
        //}
        return true;
    }


    void ClangASTVisitor::addReflectableEntity(clang::FunctionDecl* pFnDecl, const std::string& pHeader)
    {
        std::vector<std::string> parmTypes;
        std::vector<std::string> headers = { m_preProcessor.getIncludeStrSet().begin(),
                                             m_preProcessor.getIncludeStrSet().end() };

        const auto& params = pFnDecl->parameters();
        const auto& fnQName = pFnDecl->getQualifiedNameAsString();

        for (unsigned index = 0; index < params.size(); index++) {
            const auto& qT = params[index]->getOriginalType();
            const auto& argStr = ASTDeclsUtils::extractQualifiedTypeName(qT);
            if (shouldBeExcluded(argStr)) {
                return;
            }
            parmTypes.push_back(argStr);
            //if (auto incf = ASTDeclsUtils::resolveHeaderFromType(qT, pFnDecl->getASTContext(), m_preProcessor)) {
            //    headers.push_back(*incf);
            //}
            //else {
            //    Logger::outDbg("header not found for type (arg): " + parmTypes.back());
            //}
        }

        const auto& qT = pFnDecl->getReturnType();
        const auto returnStr = ASTDeclsUtils::extractQualifiedTypeName(pFnDecl->getReturnType());
        //if (auto incf = ASTDeclsUtils::resolveHeaderFromType(qT, pFnDecl->getASTContext(), m_preProcessor)) {
        //    headers.push_back(*incf);
        //}
        //else {
        //    Logger::outDbg("header not found for type (return): " + returnStr);
        //}

        std::string functionName;
        MetaKind metaKind = MetaKind::None;

        if (const auto* ctor = llvm::dyn_cast<clang::CXXConstructorDecl>(pFnDecl))
        {
            if (ctor->isUserProvided() && !ctor->isDefaultConstructor() &&
                !ctor->isCopyConstructor() && !ctor->isMoveConstructor()) {
                metaKind = MetaKind::Ctor;
            }
        }
        else if (const auto* method = llvm::dyn_cast<clang::CXXMethodDecl>(pFnDecl))
        {
            if (method->isOverloadedOperator() || llvm::isa<clang::CXXConversionDecl>(method)) {
                return;
            }

            if (method->isStatic()) {
                metaKind = MetaKind::MemberFnStatic;
            }
            else {
                if (method->isConst()) {
                    metaKind = MetaKind::MemberFnConst;
                }
                else {
                    metaKind = MetaKind::MemberFnNonConst;
                }
            }
            functionName = pFnDecl->getDeclName().getAsString();
        }
        else {
            metaKind = MetaKind::NonMemberFn;
            functionName = pFnDecl->getQualifiedNameAsString();
        }

        if (metaKind != MetaKind::None) {
            
            const std::string recordStr = ASTDeclsUtils::extractParentTypeName(pFnDecl);

            if (shouldBeExcluded(functionName) || 
                shouldBeExcluded(returnStr) || 
                shouldBeExcluded(recordStr)) {
                return;
            }

            ASTCodeManager::instance().getCodeBuffer(m_srcFile, true)
                                      ->addFunction(metaKind, {
                    .headers = headers,
                    .function = functionName
            }, recordStr, returnStr, StringUtils::getParamTypesStr(parmTypes));
        }
    }
}