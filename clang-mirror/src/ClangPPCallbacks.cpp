
#include <queue>

#include "ClangPPCallbacks.h"

using namespace clang;

namespace clmr {

    ClangPPCallbacks::ClangPPCallbacks(SourceManager& SM, CompilerInstance& CI)
        : m_srcMgr(SM)
        , m_compiler(CI)
    { }


    bool ClangPPCallbacks::isFileReachableFromHeader(const FileEntry *pHeaderFE,
                                                     const FileEntry* pFile) 
    {
        if (!pHeaderFE || !pFile) {
            return false;
        }

        std::queue<const FileEntry*> fileEntryQ;
        std::unordered_set<const FileEntry*> visited;

        fileEntryQ.push(pHeaderFE);
        while (!fileEntryQ.empty()) 
        {
            auto* nextFile = fileEntryQ.front();
            fileEntryQ.pop();

            if (!visited.insert(nextFile).second) {
                continue;
            }

            auto itr = m_inclusionGraph.find(nextFile);
            if(itr == m_inclusionGraph.end()){
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
        auto includingFileID = SM.getFileID(pHashLoc);
        auto* includingFile = SM.getFileEntryForID(includingFileID);
        if (!includingFile) {
            return;
        }
        m_inclusionGraph[includingFile].insert(&pFile->getFileEntry());
	}
}