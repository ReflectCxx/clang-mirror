#pragma once

#include <set>
#include <string_view>
#include <unordered_map>

#include "Constants.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Frontend/CompilerInstance.h"

namespace clmr {

    struct FileComparator
    {
        bool operator()(const clang::FileEntry*, const clang::FileEntry*) const;
    };
}

namespace clmr {

    class ClangPPCallbacks : public clang::PPCallbacks
    {
        clang::CompilerInstance& m_compiler;
        const clang::FileEntry* m_mainSrcFile;

        using IncludeFESet = std::set<const clang::FileEntry*, FileComparator>;
        using IncludeStrMap = std::unordered_map<const clang::FileEntry*, std::string>;

        IncludeStrMap m_includeStrMap;
        std::unordered_map<const clang::FileEntry*, IncludeFESet> m_includeGraph;

        bool isSystemHeader(const clang::FileEntry*) const;

    public:

        ClangPPCallbacks(clang::CompilerInstance& CI);

        const clang::FileEntry* getFileDoingHashIncludeFor(const clang::FileEntry* pFile) const;

        std::optional<std::string> getHashIncludeAsWritten(const clang::FileEntry* pFile,
                                                           bool pSkipSystemHeader) const;

        const clang::FileEntry* isHeaderReachableFromSrc(const clang::FileEntry *pHeader,
                                                         const clang::FileEntry* pFile) const;

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