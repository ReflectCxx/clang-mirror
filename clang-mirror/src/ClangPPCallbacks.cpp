
#include "ClangPPCallbacks.h"

namespace clmr {

    ClangPPCallbacks::ClangPPCallbacks(clang::SourceManager& SM)
        : m_srcMgr(SM)
    { }

    void ClangPPCallbacks::InclusionDirective(clang::SourceLocation HashLoc,
                                              const clang::Token& IncludeTok, llvm::StringRef FileName,
                                              bool IsAngled, clang::CharSourceRange FilenameRange,
                                              clang::OptionalFileEntryRef File,
                                              llvm::StringRef SearchPath, llvm::StringRef RelativePath,
                                              const clang::Module* SuggestedModule,
                                              bool ModuleImported,
                                              clang::SrcMgr::CharacteristicKind FileType) {
        if (!File) {
            return;
        }

        std::string headerIncStr = IsAngled ? "<" + FileName.str() + ">"
                                            : "\"" + FileName.str() + "\"";

        m_includeStrMap[*File] = headerIncStr;
	}
}