
#include "CLPPCallbacks.h"

namespace clmr {

    CLPPCallbacks::CLPPCallbacks(clang::SourceManager& SM)
        : SM(SM)
    { }

    void CLPPCallbacks::InclusionDirective(clang::SourceLocation HashLoc,
                                           const clang::Token& IncludeTok, llvm::StringRef FileName,
                                           bool IsAngled, clang::CharSourceRange FilenameRange,
                                           clang::OptionalFileEntryRef File,
                                           llvm::StringRef SearchPath, llvm::StringRef RelativePath,
                                           const clang::Module* SuggestedModule,
                                           bool ModuleImported,
                                           clang::SrcMgr::CharacteristicKind FileType)
	{

	}
}