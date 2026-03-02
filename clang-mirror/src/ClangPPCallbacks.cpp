
#include <queue>
#include <unordered_set>

#include "Logger.h"
#include "ClangPPCallbacks.h"
#include "ClangASTVisitor.hpp"

using namespace clang;

namespace {

    llvm::StringRef getRealPath(const clang::FileEntry* pFE)
    {
        auto realPath = pFE->tryGetRealPathName().str();
        if (realPath.empty()) {
            clmr::Logger::outError("clang::FileEntry::tryGetRealPathName() -> failed.");
        }
        return realPath;
    }
}

namespace clmr {

    bool FileComparator::operator()(const clang::FileEntry *pFEa, const clang::FileEntry *pFEb) const
    {
        bool aIsHeader = isHeaderFile(getRealPath(pFEa).str());
        bool bIsHeader = isHeaderFile(getRealPath(pFEa).str());
        if (aIsHeader != bIsHeader) {
            return aIsHeader > bIsHeader;
        }
        return std::less<const clang::FileEntry*> { } (pFEa, pFEb);
    }
}

namespace clmr {

    ClangPPCallbacks::ClangPPCallbacks(CompilerInstance& CI)
        : m_compiler(CI)
    {
        const auto& SM = CI.getSourceManager();
        m_mainSrcFile = SM.getFileEntryForID(SM.getMainFileID());
    }


    const clang::FileEntry* ClangPPCallbacks::getFileDoingHashIncludeFor(const clang::FileEntry* pFile) const
    {
        auto& headersSet = m_includeGraph.find(m_mainSrcFile)->second;
        for (auto* headerFile : headersSet) {
            if (auto* userDefHeader = isHeaderReachableFromSrc(headerFile, pFile)) {
                if (isPublicHeader(userDefHeader)) {
                    return userDefHeader;
                }
            }
        }
        return nullptr;
    }


    bool ClangPPCallbacks::isSystemHeader(const clang::FileEntry* pFile) const
    {
        const auto& SM = m_compiler.getSourceManager();
        const auto& fileID = SM.translateFile(pFile);
        if (!fileID.isValid()) {
            return false;
        }

        SourceLocation Loc = SM.getLocForStartOfFile(fileID);
        if (SM.getFileCharacteristic(Loc) == SrcMgr::C_User) {
            return false;
        }
        return true;
    }


    std::optional<std::string> ClangPPCallbacks::getHashIncludeAsWritten(const FileEntry *pIncFile,
                                                                         bool pSkipSystemHeader) const
    {
        if (pSkipSystemHeader && isSystemHeader(pIncFile)) {
            return std::nullopt;
        }

        const auto& itr = m_includeStrMap.find(pIncFile);
        if (itr == m_includeStrMap.end()) {
            return std::nullopt;
        }
        return std::make_optional(itr->second);
    }


    const FileEntry* ClangPPCallbacks::isHeaderReachableFromSrc(const FileEntry* pIncSrc,
                                                                const FileEntry* pHeader) const
    {
        if (!pIncSrc || !pHeader) {
            return pHeader;
        }

        if (pIncSrc == pHeader) {
            return pHeader;
        }

        std::queue<const FileEntry*> fileEntryQ;
        std::unordered_set<const FileEntry*> visited;
        fileEntryQ.push(pIncSrc);

        const clang::FileEntry* lastUserFile = pIncSrc;
        while (!fileEntryQ.empty())
        {
            auto* nextFile = fileEntryQ.front();
            fileEntryQ.pop();

            if (!isSystemHeader(nextFile)) {
                lastUserFile = nextFile;
            }

            if (!visited.insert(nextFile).second) {
                continue;
            }

            auto itr = m_includeGraph.find(nextFile);
            if(itr == m_includeGraph.end()) {
                continue;
            }

            auto& nextIncludesSet = itr->second;
            if (nextIncludesSet.find(pHeader) != nextIncludesSet.end()){
                return lastUserFile;
            }
            for (auto* fe : nextIncludesSet) {
                fileEntryQ.push(fe);
            }
        }
        return nullptr;
    }


    void ClangPPCallbacks::InclusionDirective(SourceLocation pHashLoc,
                                              const Token &pIncludeTok, llvm::StringRef pFileName,
                                              bool pIsAngled, CharSourceRange pFilenameRange,
                                              OptionalFileEntryRef pFile,
                                              llvm::StringRef pSearchPath, llvm::StringRef pRelativePath,
                                              const Module *pSuggestedModule,
                                              bool pModuleImported,
                                              SrcMgr::CharacteristicKind pFileType)
    {
        if (!pFile) {
            return;
        }

        auto& SM = m_compiler.getSourceManager();
        auto* incSrcFile = SM.getFileEntryForID(SM.getFileID(pHashLoc));
        if (!incSrcFile) {
            return;
        }

        auto headerFile = &pFile->getFileEntry();
        m_includeGraph[incSrcFile].insert(headerFile);
        m_includeStrMap[*pFile] = pIsAngled ? ("<" + pFileName.str() + ">")
                                            : ("\"" + pFileName.str() + "\"");
	}
}