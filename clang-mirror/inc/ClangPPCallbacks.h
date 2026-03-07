#pragma once

#include <set>
#include <vector>
#include <string_view>
#include <unordered_map>

#include "Constants.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Frontend/CompilerInstance.h"

namespace clmr {

    struct FileComparator
    {
        const clang::FileEntry& m_mainSrcRef;
        bool operator()(const clang::FileEntry*, const clang::FileEntry*) const;
    };
}

namespace clmr {

    class ClangPPCallbacks : public clang::PPCallbacks
    {
        clang::CompilerInstance& m_compiler;
        const clang::FileEntry* m_mainSrcFile;

        using IncludeFESet = std::set<const clang::FileEntry*, FileComparator>;
        std::unordered_map<const clang::FileEntry*, IncludeFESet> m_includeGraph;
        std::unordered_map<const clang::FileEntry*, std::string> m_includeStrMap;

        void buildIncludeStack(std::vector<const clang::FileEntry*>& pIncStack, const clang::FileEntry* pHeader) const;

        std::pair<RegErr, const clang::FileEntry*> getFileDoingHashIncludeFor(const clang::FileEntry* pHeader) const;

    public:

        ClangPPCallbacks(clang::CompilerInstance& CI);

        bool isSystemHeader(const clang::FileEntry* pFile) const;

        std::pair<RegErr, std::string> getHashIncludeAsWrittenFor(const clang::FileEntry* pHeader) const;

        void InclusionDirective(clang::SourceLocation HashLoc,
                                const clang::Token& IncludeTok, llvm::StringRef FileName,
                                bool IsAngled, clang::CharSourceRange FilenameRange,
                                clang::OptionalFileEntryRef File,
                                llvm::StringRef SearchPath, llvm::StringRef RelativePath,
                                const clang::Module* SuggestedModule,
                                bool ModuleImported,
                                clang::SrcMgr::CharacteristicKind FileType) override;
    };
}