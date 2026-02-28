#pragma once

#include <string>
#include <filesystem>

#include "Logger.h"
#include "ASTCodeManager.h"
#include "clang/AST/RecursiveASTVisitor.h"

namespace {

    static bool isBuiltInType(const clang::QualType& pQT)
    {
        auto qT = pQT.getNonReferenceType().getUnqualifiedType();
        if (qT->isPointerType()) {
            qT = qT->getPointeeType();
        }
        return qT->isBuiltinType();
    }

    static bool isHeaderFile(const std::string& pFileStr)
    {
        const auto& ext = llvm::sys::path::extension(pFileStr);
        return ext.equals_insensitive(".h") ||
               ext.equals_insensitive(".hpp") ||
               ext.equals_insensitive(".hh")  ||
               ext.equals_insensitive(".hxx") ||
               ext.equals_insensitive(".inl") ||
               ext.equals_insensitive(".inc");
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

    static const clang::FileEntry* getDeclaringFile(clang::FunctionDecl *pFnDecl)
    {
        auto& SM = pFnDecl->getASTContext().getSourceManager();
        for (auto* decl : pFnDecl->redecls())
        {
            clang::SourceLocation loc = SM.getSpellingLoc(decl->getLocation());
            const auto& fileStr = SM.getFilename(loc).str();
            if (isHeaderFile(fileStr)) {
                clang::FileID fid = SM.getFileID(loc);
                return SM.getFileEntryForID(fid);
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
        return true;
    }
}