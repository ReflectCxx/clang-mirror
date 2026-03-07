
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


    std::pair<RegErr, std::string> ClangPPCallbacks::getHashIncludeAsWrittenFor(const FileEntry* pHeader) const
    {
        auto [err, incFile] = getFileDoingHashIncludeFor(pHeader);
        if (err != RegErr::None) {
            return { err, {} };
        }

        const auto& itr = m_includeStrMap.find(incFile);
        if (itr == m_includeStrMap.end()) {
            Logger::outDbg("`#include` not found in file : " + getRealPath(incFile).str());
            return { RegErr::AstParsing, {} };
        }
        return { err, itr->second };
    }


    void ClangPPCallbacks::buildIncludeStack(std::vector<const clang::FileEntry*>& pIncludeStack, const clang::FileEntry* pHeader) const
    {
        std::unordered_set<const FileEntry*> visited = { pIncludeStack.front() };

        while (!pIncludeStack.empty())
        {
            auto* top = pIncludeStack.back();
            if (top == pHeader) {
                return;
            }

            auto itr = m_includeGraph.find(top);
            if (itr == m_includeGraph.end()) {
                pIncludeStack.pop_back();
                continue;
            }

            bool pop = true;
            for (auto* incSrcFile : itr->second) {
                bool unvisited = visited.insert(incSrcFile).second;
                if (unvisited) {
                    pop = false;
                    pIncludeStack.push_back(incSrcFile);
                    break;
                }
            }
            if (pop) pIncludeStack.pop_back();
        }
        return;
    }


    std::pair<RegErr, const clang::FileEntry*> ClangPPCallbacks::getFileDoingHashIncludeFor(const FileEntry* pHeader) const
    {
        if (!pHeader) {
            return { RegErr::AstParsing, nullptr };
        }

        auto itr = m_includeGraph.find(m_mainSrcFile);
        if (itr == m_includeGraph.end()) {
            Logger::outDbg("Header not included by source for file: " + getRealPath(pHeader).str());
            return { RegErr::AstParsing, nullptr };
        }

        bool notPublic = false;
        for (auto nxtIncFile : itr->second)
        {
            std::vector<const clang::FileEntry*> incStack = { nxtIncFile };
            buildIncludeStack(incStack, pHeader);

            if (incStack.back() != pHeader)
                continue;

            for (auto incFile : incStack) {
                if (isSystemHeader(incFile) || isPublicHeader(incFile))
                    return { RegErr::None, incFile };
                notPublic = true;
            }
        }

        if (notPublic) {
            Logger::outDbg("not found at specified `include` dir: " + getRealPath(pHeader).str());
            return { RegErr::HeaderNotPublic, nullptr };
        }
        Logger::outDbg("Header not reachable for file: " + getRealPath(pHeader).str());
        return { RegErr::AstParsing, nullptr };
    }


    void ClangPPCallbacks::InclusionDirective(SourceLocation pHashLoc,
                                              const Token &pIncludeTok, llvm::StringRef pFileName,
                                              bool pIsAngled, CharSourceRange pFilenameRange,
                                              OptionalFileEntryRef pFile,
                                              llvm::StringRef pSearchPath, llvm::StringRef pRelativePath,
                                              const Module *pSuggestedModule,
                                              bool pModuleImported,
                                              SrcMgr::CharacteristicKind pFileType) {
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