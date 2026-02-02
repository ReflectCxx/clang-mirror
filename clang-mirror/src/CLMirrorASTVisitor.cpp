
#include <iostream>
#include <algorithm>

#include "Constants.h"
#include "StringUtils.h"
#include "ASTCodeManager.h"
#include "ASTCodeBuffer.h"
#include "ASTDeclsUtils.h"
#include "CLMirrorASTVisitor.h"

using namespace clang;

namespace clmirror
{
    CLMirrorASTVisitor::CLMirrorASTVisitor(const std::string& pSrcFile)
        : m_srcFile(pSrcFile)
	{ }


    bool CLMirrorASTVisitor::VisitFunctionDecl(FunctionDecl* pFnDecl)
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

        std::string declSrcFile;
        auto& SM = pFnDecl->getASTContext().getSourceManager();
        for (auto* D : pFnDecl->redecls())
        {
            SourceLocation loc = SM.getSpellingLoc(D->getLocation());
            if (!loc.isValid()) {
                continue;
            }

            StringRef fileName = SM.getFilename(loc);
            if (fileName.ends_with(".h") || fileName.ends_with(".hpp"))
            {
                declSrcFile = fileName.str();
                break;
            }
        }

        if (!declSrcFile.empty()) 
        {
            std::vector<std::string> parmTypes;
            const auto& params = pFnDecl->parameters();
            const auto& fnQName = pFnDecl->getQualifiedNameAsString();
            for (unsigned index = 0; index < params.size(); index++) {
                parmTypes.push_back(ASTDeclsUtils::extractParameterType(params[index]));
            }

            std::string functionName;
            MetaKind metaKind = MetaKind::None;

            if (llvm::isa<clang::CXXConstructorDecl>(pFnDecl))
            {
                metaKind = MetaKind::Ctor;
                functionName = pFnDecl->getDeclName().getAsString();
            }
            else if (const auto* method = llvm::dyn_cast<clang::CXXMethodDecl>(pFnDecl))
            {
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

            auto codeBuffer = ASTCodeManager::instance().getCodeBuffer(m_srcFile, true);
            const std::string recordStr = ASTDeclsUtils::extractParentTypeName(pFnDecl);

            codeBuffer->addFunction(metaKind, declSrcFile, recordStr, functionName, StringUtils::getSignatureStr(parmTypes));
        }
        return true;
    }
}