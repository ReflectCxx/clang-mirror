
#include <queue>
#include <unordered_set>

#include "Logger.h"
#include "ClangPPCallbacks.h"
#include "ClangASTVisitor.hpp"

using namespace clang;


namespace clmr {

    bool FileComparator::operator()(const FileEntry *pFEa, const FileEntry *pFEb) const
    {
        auto pathA = getRealPath(pFEa);
        auto pathB = getRealPath(pFEb);

        const std::string mainFilePath = getRealPath(&m_mainSrcRef).str();
        const std::string stemA = std::filesystem::path(pathA.str()).stem().string();
        const std::string stemB = std::filesystem::path(pathB.str()).stem().string();
        const std::string stemMain = std::filesystem::path(mainFilePath).stem().string();

        const bool aIsMain = (stemA == stemMain);
        const bool bIsMain = (stemB == stemMain);
        if (aIsMain != bIsMain) {
            return aIsMain;
        }

        bool aIsHeader = isHeaderFile(getRealPath(pFEa));
        bool bIsHeader = isHeaderFile(getRealPath(pFEb));
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


    const FileEntry* ClangPPCallbacks::getFileDoingHashIncludeFor(const FileEntry* pHeader) const
    {
        auto includeChain = getIncludeChainFromSrcToHeader(pHeader);
        for (int i = 1; i < includeChain.size(); i++) {
            if(isSystemHeader(includeChain[i]) || isPublicHeader(includeChain[i])) {
                return includeChain[i];
            }
        }
        Logger::outDbg("Header not reachable for file: " + getRealPath(pHeader).str());
        return nullptr;
    }


    std::optional<std::string> ClangPPCallbacks::getHashIncludeAsWrittenFor(const FileEntry * pHeader) const
    {
        const auto* incFile = getFileDoingHashIncludeFor(pHeader);
        if (!incFile) {
            return std::nullopt;
        }

        const auto& itr = m_includeStrMap.find(incFile);
        if (itr == m_includeStrMap.end()) {
            Logger::outDbg("`#include` not found in file : " + getRealPath(incFile).str());
            return std::nullopt;
        }
        return std::make_optional(itr->second);
    }


    std::vector<const FileEntry*> ClangPPCallbacks::getIncludeChainFromSrcToHeader(const FileEntry* pHeader) const
    {
        if (!pHeader) {
            return {};
        }

        std::vector<const FileEntry*> includeStack = { m_mainSrcFile };
        std::unordered_set<const FileEntry*> visited = { m_mainSrcFile };

        while (!includeStack.empty())
        {
            auto* topHeader = includeStack.back();
            if (topHeader == pHeader) {
                return includeStack;
            }

            auto itr = m_includeGraph.find(topHeader);
            if (itr == m_includeGraph.end()) {
                includeStack.pop_back();
                continue;
            }

            bool advance = true;
            for (auto* incSrcFile : itr->second) {
                bool unvisited = visited.insert(incSrcFile).second;
                if (unvisited) {
                    advance = false;
                    includeStack.push_back(incSrcFile);
                    break;
                }
            }

            if (advance) {
                includeStack.pop_back();
            }
        }
        return {};
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
        
        if (pIsAngled) {
            m_includeStrMap[*pFile] = ("<" + pFileName.str() + ">");
        }
        else {
            m_includeStrMap[*pFile] = ("\"" + pFileName.str() + "\"");
        }

        auto [itr, inserted] = m_includeGraph.try_emplace( 
            incSrcFile,
            FileComparator{ *m_mainSrcFile }
        );

        auto headerFile = &pFile->getFileEntry();
        itr->second.insert(headerFile);
	}
}