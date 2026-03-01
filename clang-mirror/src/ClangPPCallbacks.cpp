
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


    bool ClangPPCallbacks::isFileReachableFromHeader(const FileEntry* pMainHeader,
                                                     const FileEntry* pFile) 
    {
        if (!pMainHeader || !pFile) {
            return false;
        }

        std::queue<const FileEntry*> fileEntryQ;
        std::unordered_set<const FileEntry*> visited;

        fileEntryQ.push(pMainHeader);
        while (!fileEntryQ.empty()) 
        {
            auto* nextFile = fileEntryQ.front();
            fileEntryQ.pop();

            if (!visited.insert(nextFile).second) {
                continue;
            }

            auto itr = m_inclusionGraph.find(nextFile);
            if(itr == m_inclusionGraph.end()) {
                continue;
            }

            auto& nextIncludesSet = itr->second;
            if (nextIncludesSet.find(pFile) != nextIncludesSet.end()){
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

        std::string headerIncStr = pIsAngled ? "<" + pFileName.str() + ">"
                                             : "\"" + pFileName.str() + "\"";
        m_includeStrMap[*pFile] = headerIncStr;

        auto& SM = m_compiler.getSourceManager();
        auto* hashIncludeLocFile = SM.getFileEntryForID(SM.getFileID(pHashLoc));
        if (!hashIncludeLocFile) {
            return;
        }
        m_inclusionGraph[hashIncludeLocFile].insert(&pFile->getFileEntry());
	}
}