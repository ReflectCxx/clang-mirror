#pragma once

#include <string>
#include <filesystem>
#include "ASTCodeManager.h"

#include "clang/AST/RecursiveASTVisitor.h"

namespace {

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

    static bool taggedForExclusion(const std::string& pStr)
    {
        const auto& exclusions = clmr::ASTCodeManager::instance().getExcludeNamespaces();
        for (const auto& excStr : exclusions) {
            if (pStr.find(excStr + "::") != std::string::npos ||
                pStr.find('<') != std::string::npos) {  // exclude templates as well. (not supported yet)
                return true;
            }
        }
        return false;
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

    static bool isPathRelativeToBase(const std::string& pBasePath, const std::string& pOther)
    {
        namespace fs = std::filesystem;
        fs::path basePath = fs::weakly_canonical(fs::path(pBasePath));
        fs::path otherPath = fs::weakly_canonical(fs::path(pOther));
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
        for (auto& incPath : publicIncPaths) {
            auto realPath = file->tryGetRealPathName().str();
            if(isPathRelativeToBase(incPath, realPath)) {
                return true;
            }
        }
        return false;
    }
}