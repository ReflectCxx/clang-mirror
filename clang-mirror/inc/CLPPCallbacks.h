#pragma once

#include "clang/Lex/PPCallbacks.h"

namespace clmr {

    class CLPPCallbacks : public clang::PPCallbacks
    {
        clang::SourceManager& m_srcMgr;
        std::unordered_map<const clang::FileEntry*, std::string> m_headerMap;

    public:

        CLPPCallbacks(clang::SourceManager& SM);

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
