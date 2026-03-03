
#include <queue>
#include <unordered_set>

#include "Logger.h"
#include "ClangPPCallbacks.h"
#include "ClangASTVisitor.hpp"

using namespace clang;

namespace clmr {

    bool FileComparator::operator()(const FileEntry *pFEa, const FileEntry *pFEb) const
    {
        bool aIsHeader = isHeaderFile(getRealPath(pFEa).str());
        bool bIsHeader = isHeaderFile(getRealPath(pFEa).str());
        if (aIsHeader != bIsHeader) {
            return aIsHeader > bIsHeader;
        }
        return std::less<const FileEntry*> { } (pFEa, pFEb);
    }
}

namespace clmr {

    ClangPPCallbacks::ClangPPCallbacks(CompilerInstance& CI)
        : m_compiler(CI)
    {
        const auto& SM = CI.getSourceManager();
        m_mainSrcFile = SM.getFileEntryForID(SM.getMainFileID());
    }


    std::optional<std::string> ClangPPCallbacks::getHashIncludeAsWritten(const FileEntry *pIncFile) const
    {
        const auto& itr = m_includeStrMap.find(pIncFile);
        if (itr == m_includeStrMap.end()) {
            return std::nullopt;
        }
        return std::make_optional(itr->second);
    }


    const FileEntry* ClangPPCallbacks::getFileDoingHashIncludeFor(const FileEntry* pHeader) const
    {
        auto& incSrcSet = m_includeGraph.find(m_mainSrcFile)->second;
        for (auto* incSrcFile : incSrcSet) {
            if (isHeaderReachableFromSrc(incSrcFile, pHeader)) {
                if (isSystemHeader(incSrcFile) || isPublicHeader(incSrcFile)) {
                    return incSrcFile;
                }
            }
        }
        return nullptr;
    }


    bool ClangPPCallbacks::isSystemHeader(const FileEntry* pFile) const
    {
        const auto& SM = m_compiler.getSourceManager();
        const auto& fileID = SM.translateFile(pFile);
        if (!fileID.isValid()) {
            return false;
        }

        SourceLocation Loc = SM.getLocForStartOfFile(fileID);
        return (SM.getFileCharacteristic(Loc) != SrcMgr::C_User);
    }


    bool ClangPPCallbacks::isHeaderReachableFromSrc(const FileEntry* pIncSrc, const FileEntry* pHeader) const
    {
        if (!pIncSrc || !pHeader) {
            return false;
        }

        if (pIncSrc == pHeader) {
            return true;
        }

        std::queue<const FileEntry*> fileEntryQ;
        std::unordered_set<const FileEntry*> visited;
        fileEntryQ.push(pIncSrc);

        while (!fileEntryQ.empty())
        {
            auto* nextFile = fileEntryQ.front();
            fileEntryQ.pop();

            if (!visited.insert(nextFile).second) {
                continue;
            }

            auto itr = m_includeGraph.find(nextFile);
            if(itr == m_includeGraph.end()) {
                continue;
            }

            auto& nextIncludesSet = itr->second;
            if (nextIncludesSet.find(pHeader) != nextIncludesSet.end()){
                return true;
            }
            for (auto* fe : nextIncludesSet) {
                fileEntryQ.push(fe);
            }
        }
        return false;
    }

    std::vector<const FileEntry*> ClangPPCallbacks::getIncludeChainFromSrcToHeader(const FileEntry* pIncSrc, const FileEntry* pHeader) const
    {
        std::vector<const FileEntry*> includeStack;
        std::unordered_set<const FileEntry*> visited;
        return includeStack;
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

        if (incSrcFile == m_mainSrcFile) {
            m_includeStrMap[*pFile] = pIsAngled ? ("<" + pFileName.str() + ">")
                                                : ("\"" + pFileName.str() + "\"");
        }
	}
}