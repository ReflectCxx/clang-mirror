#pragma once

#include <string>
#include <filesystem>

#include "Logger.h"
#include "ASTCodeManager.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace {

    static llvm::StringRef getRealPath(const clang::FileEntry* pFE)
    {
        auto realPath = pFE->tryGetRealPathName().str();
        if (realPath.empty()) {
            clmr::Logger::outError("FileEntry::tryGetRealPathName() -> failed.");
        }
        return realPath;
    }

    static const clang::QualType desugarQT(const clang::QualType& pQT, 
                                           const clang::ASTContext& pCtx)
    {
        auto qT = pQT.getNonReferenceType()
                     .getUnqualifiedType()
                     .getDesugaredType(pCtx);

        while (qT->isPointerType()) {
            qT = qT->getPointeeType().getDesugaredType(pCtx);
        }
        return qT;
    }

    static bool isHeaderFile(const std::string& pFileStr)
    {
        const auto& ext = llvm::sys::path::extension(pFileStr);
        return ext.equals_insensitive(".h") ||
               ext.equals_insensitive(".hpp") ||
               ext.equals_insensitive(".hh")  ||
               ext.equals_insensitive(".hxx");
    }

    static clmr::RegErr taggedForExclusion(const std::string& pStr)
    {
        if (pStr.find('<') != std::string::npos) {  // exclude templates. (not supported yet)
            return clmr::RegErr::TemplateType;
        }
        const auto& exclusions = clmr::ASTCodeManager::instance().getExcludeNamespaces();
        for (const auto& excStr : exclusions) {
            if (pStr.find(excStr + "::") != std::string::npos) {
                return clmr::RegErr::ExclusionByPolicy;
            }
        }
        return clmr::RegErr::None;
    }

    static const clang::FileEntry* getDeclaringFile(clang::FunctionDecl * pFnDef)
    {
        if (!pFnDef) {
            return nullptr;
        }

        auto& SM = pFnDef->getASTContext().getSourceManager();
        for (auto* redecl : pFnDef->redecls())
        {
            if (redecl == pFnDef) continue;
            clang::SourceLocation loc = SM.getExpansionLoc(redecl->getLocation());

            if (loc.isInvalid()) continue;
            if (SM.isInMainFile(loc)) continue;
            if (SM.isInSystemHeader(loc)) continue;

            clang::FileID fid = SM.getFileID(loc);
            const clang::FileEntry* file = SM.getFileEntryForID(fid);

            if (!file) continue;
            auto fpath = file->tryGetRealPathName();

            if (fpath.empty()) continue;
            if (isHeaderFile(fpath.str())) {
                return file;
            }
        }
        return nullptr;
    }

    static bool isSubDirectoryOf(const std::string& pBaseDir, const std::string& pSubDir)
    {
        namespace fs = std::filesystem;
        fs::path basePath = fs::weakly_canonical(fs::path(pBaseDir));
        fs::path otherPath = fs::weakly_canonical(fs::path(pSubDir));
        basePath = basePath.lexically_normal();
        otherPath = otherPath.lexically_normal();

        auto bItr = basePath.begin();
        auto oItr = otherPath.begin();
        for (; bItr != basePath.end() && oItr != otherPath.end(); ++bItr, ++oItr) {
            if (*bItr != *oItr){
                return false;
            }
        }
        return bItr == basePath.end();
    }

    static bool isPublicHeader(const clang::FileEntry* file)
    {
        const auto& publicIncPaths = clmr::ASTCodeManager::instance().getPublicIncludePaths();
        if (publicIncPaths.empty()) {
            return true;
        }
        for (auto& incPath : publicIncPaths) {
            auto realPath = file->tryGetRealPathName().str();
            if (realPath.empty()) {
                clmr::Logger::outError("clang::FileEntry::tryGetRealPathName() -> failed.");
                continue;
            }
            if (isSubDirectoryOf(incPath, realPath)) {
                return true;
            }
        }
        return false;
    }

    bool isReflectableEntity(clang::FunctionDecl* pFnDecl)
    {
        if ( pFnDecl->getDefinition() != pFnDecl ||
             pFnDecl->isDeleted() ||
             pFnDecl->isImplicit() ||
             pFnDecl->isOverloadedOperator() ||
             pFnDecl->isFunctionTemplateSpecialization() ||
             pFnDecl->getLinkageInternal() != clang::Linkage::External ||
             isa<clang::CXXConversionDecl>(pFnDecl) || isa<clang::CXXDestructorDecl>(pFnDecl) ||
            (isa<clang::CXXMethodDecl>(pFnDecl) && cast<clang::CXXMethodDecl>(pFnDecl)->getAccess() != clang::AS_public)) {
            return false;
        }

        auto& SM = pFnDecl->getASTContext().getSourceManager();
        clang::SourceLocation loc = SM.getExpansionLoc(pFnDecl->getLocation());
        if (loc.isInvalid() || !SM.isInMainFile(loc)) {
            return false;
        }

        auto* ctor = llvm::dyn_cast<clang::CXXConstructorDecl>(pFnDecl);
        if (ctor && (ctor->getNumParams() == 0 ||
            ctor->isCopyConstructor() || ctor->isMoveConstructor())) {
            return false;
        }

        auto* method = llvm::dyn_cast<clang::CXXMethodDecl>(pFnDecl);
        if (method) {
            if(method->isOverloadedOperator()) {
                return false;
            }
            auto* record = method->getParent();
            if (record->getAccess() == clang::AS_private || record->getAccess() == clang::AS_protected) {
                return false;
            }
        }
        return true;
    }
}