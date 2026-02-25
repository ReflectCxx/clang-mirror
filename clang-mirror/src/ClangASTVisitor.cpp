
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

    bool isHeaderFile(const std::string& pFileStr)
    {
        const auto& ext = llvm::sys::path::extension(pFileStr);
        return ext.equals_insensitive(".h") ||
               ext.equals_insensitive(".hpp") ||
               ext.equals_insensitive(".hh")  ||
               ext.equals_insensitive(".hxx") ||
               ext.equals_insensitive(".inl") ||
               ext.equals_insensitive(".inc");
    }

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

    std::optional<std::string> getDeclHeader(FunctionDecl *pFnDecl)
    {
        auto& SM = pFnDecl->getASTContext().getSourceManager();
        for (auto* decl : pFnDecl->redecls()) 
        {
            SourceLocation loc = SM.getSpellingLoc(decl->getLocation());
            if (loc.isInvalid() || SM.isInSystemHeader(loc)) {
                continue;
            }
            const auto& fileStr = SM.getFilename(loc).str();
            if (isHeaderFile(fileStr)) {
               return fileStr;
            }
        }
        return std::nullopt;
    }
}


namespace clmr
{
    ClangASTVisitor::ClangASTVisitor(const std::string& pSrcFile, ClangPPCallbacks& pPP)
        : m_srcFile(pSrcFile)
        , m_preProcessor(pPP)
	{ }


    bool ClangASTVisitor::isHeaderReachableForType(const clang::QualType &pQT,
                                                   const clang::FunctionDecl *pFnDecl,
                                                   const std::string& pTypeStr,
                                                   const std::string& pSrcHeader)
    {
        if (auto incf = ASTDeclsUtils::resolveHeaderFromType(pQT, pFnDecl->getASTContext(), m_preProcessor)) {
            if (!m_preProcessor.isFileReachableFromHeader(pSrcHeader, incf)) {
                Logger::outDbg("header not reachable for type: " + pTypeStr);
                return false;
            }
        }
        else {
            Logger::outDbg("header not found for type: " + pTypeStr);
            return false;
        }
        return true;
    }


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

        const auto* method = llvm::dyn_cast<CXXMethodDecl>(pFnDecl);
        if (method) {
            const CXXRecordDecl* record = method->getParent();
            if (record->getAccess() != AS_public) {
                return true;
            }
        }

        auto declHeader = getDeclHeader(pFnDecl);
        if (declHeader) {
            addReflectableEntity(pFnDecl, *declHeader);
        }
        return true;
    }


    void ClangASTVisitor::addReflectableEntity(FunctionDecl *pFnDecl, const std::string &pHeader)
    {
        std::vector<std::string> parmTypes;
        const auto& params = pFnDecl->parameters();
        const auto& fnQName = pFnDecl->getQualifiedNameAsString();

        for (unsigned index = 0; index < params.size(); index++)
        {
            const auto& qT = params[index]->getOriginalType();
            const auto& argStr = ASTDeclsUtils::extractQualifiedTypeName(qT);
            if ( shouldBeExcluded(argStr) ||
                !isHeaderReachableForType(qT, pFnDecl, argStr, pHeader)) {
                return;
            }
            parmTypes.push_back(argStr);
        }

        const auto& qT = pFnDecl->getReturnType();
        const auto returnStr = ASTDeclsUtils::extractQualifiedTypeName(pFnDecl->getReturnType());
        if (isHeaderReachableForType(qT, pFnDecl, returnStr, pHeader)){
            return;
        }

        auto [metaKind, fname] = ASTDeclsUtils::getNameAndMetaKind(pFnDecl);
        if (metaKind == MetaKind::None) return;

        const std::string recordStr = ASTDeclsUtils::extractParentTypeName(pFnDecl);
        if (shouldBeExcluded(fname) || 
            shouldBeExcluded(returnStr) || 
            shouldBeExcluded(recordStr)) {
            return;
        }

        auto* codeBuffer = ASTCodeManager::instance().getCodeBuffer(m_srcFile, true);
        codeBuffer->addFunction(metaKind, {
                .headers = { pHeader },
                .function = fname
        }, recordStr, returnStr, StringUtils::getParamTypesStr(parmTypes));
    }
}