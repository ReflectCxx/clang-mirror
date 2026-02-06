#pragma once

#include "Constants.h"
#include "clang/Lex/PPCallbacks.h"

namespace clmr {

    class ClangPPCallbacks : public clang::PPCallbacks
    {
        using IncludeStrMap = std::unordered_map<const clang::FileEntry*, std::string>;

        clang::SourceManager& m_srcMgr;
        IncludeStrMap m_includeStrMap;

    public:

        ClangPPCallbacks(clang::SourceManager& SM);

        void InclusionDirective(clang::SourceLocation HashLoc,
                                const clang::Token& IncludeTok, llvm::StringRef FileName,
                                bool IsAngled, clang::CharSourceRange FilenameRange,
                                clang::OptionalFileEntryRef File,
                                llvm::StringRef SearchPath, llvm::StringRef RelativePath,
                                const clang::Module* SuggestedModule,
                                bool ModuleImported,
                                clang::SrcMgr::CharacteristicKind FileType) override;

        GETTER_CREF(IncludeStrMap, IncludeStrMap, m_includeStrMap)
    };
}