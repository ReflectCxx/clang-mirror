
#include <iostream>
#include <algorithm>

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

        if (!ASTDeclsUtils::isDeclFrmCurrentSource(m_srcFile, pFnDecl)) {
            return true;
        }

        const auto* method = llvm::dyn_cast<clang::CXXMethodDecl>(pFnDecl);
        if (method) {
            const clang::CXXRecordDecl* record = method->getParent();
            if (record->isAbstract() || record->getAccess() != AS_public) {
                return true;
            }
        }

        auto headerStr = ASTDeclsUtils::getHeaderFileForType(pFnDecl, m_preProcessor);
        if (headerStr) {
            addReflectableEntity(pFnDecl, *headerStr);
        }
        return true;
    }


    void ClangASTVisitor::addReflectableEntity(clang::FunctionDecl* pFnDecl, const std::string& pHeader)
    {
        std::vector<std::string> parmTypes;
        const auto& params = pFnDecl->parameters();
        const auto& fnQName = pFnDecl->getQualifiedNameAsString();
        for (unsigned index = 0; index < params.size(); index++) {
            const auto& argStr = ASTDeclsUtils::extractQualifiedTypeName(params[index]->getOriginalType());
            if (shouldBeExcluded(argStr)) {
                return;
            }
            parmTypes.push_back(argStr);
        }

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
            const std::string returnStr = ASTDeclsUtils::extractQualifiedTypeName(pFnDecl->getReturnType());
            const std::string recordStr = ASTDeclsUtils::extractParentTypeName(pFnDecl);

            if (shouldBeExcluded(functionName) || 
                shouldBeExcluded(returnStr) || 
                shouldBeExcluded(recordStr)) {
                return;
            }

            ASTCodeManager::instance().getCodeBuffer(m_srcFile, true)
                                      ->addFunction(metaKind, {
                    .header = pHeader,
                    .function = functionName
            }, recordStr, returnStr, StringUtils::getParamTypesStr(parmTypes));
        }
    }
}