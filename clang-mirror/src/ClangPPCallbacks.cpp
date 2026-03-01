
#include <queue>
#include "Logger.h"
#include "ClangPPCallbacks.h"

using namespace clang;

namespace clmr {

    ClangPPCallbacks::ClangPPCallbacks(CompilerInstance& CI)
        : m_compiler(CI)
    {
        const auto& SM = CI.getSourceManager();
        m_mainSrcFile = SM.getFileEntryForID(SM.getMainFileID());
    }

    std::optional<std::string> ClangPPCallbacks::getHashIncludeAsWritten(const FileEntry *pIncFile)
    {
        const auto& itr = m_includeStrMap.find(pIncFile);
        if (itr == m_includeStrMap.end()) {
            return std::nullopt;
        }
        return std::make_optional(itr->second);
    }

    const clang::FileEntry *ClangPPCallbacks::getFileDoingHashIncludeFor(const clang::FileEntry *pFile)
    {
        auto& headersSet = m_includeGraph.find(m_mainSrcFile)->second;
        for (auto* headerFile : headersSet) {
            if (isHeaderReachableFromSrc(headerFile, pFile)) {
                return headerFile;
            }
        }
        return nullptr;
    }

    bool ClangPPCallbacks::isHeaderReachableFromSrc(const FileEntry* pIncSrc,
                                                    const FileEntry* pHeader)
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
        if (SM.getFileCharacteristic(pHashLoc) != SrcMgr::C_User) {
            return;
        }

        auto* incSrcFile = SM.getFileEntryForID(SM.getFileID(pHashLoc));
        if (!incSrcFile) {
            return;
        }

        auto headerFile = &pFile->getFileEntry();
        m_includeGraph[incSrcFile].insert(headerFile);

        std::string headerIncStr = pIsAngled ? ("<" + pFileName.str() + ">")
                                             : ("\"" + pFileName.str() + "\"");
        m_includeStrMap[*pFile] = headerIncStr;
	}
}