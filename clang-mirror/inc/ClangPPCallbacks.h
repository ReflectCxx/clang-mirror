#pragma once

#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "Constants.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Frontend/CompilerInstance.h"

namespace clmr {

    class ClangPPCallbacks : public clang::PPCallbacks
    {
        clang::CompilerInstance& m_compiler;
        const clang::FileEntry* m_mainSrcFile;

        using IncludeFESet = std::unordered_set<const clang::FileEntry*>;
        using IncludeStrMap = std::unordered_map<const clang::FileEntry*, std::string>;
        
        IncludeStrMap m_includeStrMap;        
        std::unordered_map<const clang::FileEntry*, IncludeFESet> m_inclusionGraph;

    public:

        ClangPPCallbacks(clang::CompilerInstance& CI);

        std::optional<std::string> getIncludeStrAsWritten(const clang::FileEntry* pIncFile, std::string_view pTypeStr);

        bool isFileReachableFromHeader(const clang::FileEntry *pHeaderFE,
                                       const clang::FileEntry* pFile);

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