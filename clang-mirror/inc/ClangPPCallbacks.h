#pragma once

#include "Constants.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Frontend/CompilerInstance.h"

namespace clmr {

    class ClangPPCallbacks : public clang::PPCallbacks
    {
        using IncludeStrMap = std::unordered_map<const clang::FileEntry*, std::string>;

        clang::SourceManager& m_srcMgr;
        clang::CompilerInstance& m_compiler;

        IncludeStrMap m_includeStrMap;
        std::set<std::string> m_includeStrSet;

    public:

        ClangPPCallbacks(clang::SourceManager& SM, clang::CompilerInstance& CI);

        GETTER_CREF(IncludeStrMap, IncludeStrMap, m_includeStrMap)
        GETTER_CREF(std::set<std::string>, IncludeStrSet, m_includeStrSet)

        bool isFileReachableFromHeader(const std::string& pHeader,
                                       const clang::FileEntry* pFE);

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