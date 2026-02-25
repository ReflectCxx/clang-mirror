
#include "ClangPPCallbacks.h"

namespace clmr {

    ClangPPCallbacks::ClangPPCallbacks(clang::SourceManager& SM, clang::CompilerInstance& CI)
        : m_srcMgr(SM)
        , m_compiler(CI)
    { }

    void ClangPPCallbacks::InclusionDirective(clang::SourceLocation HashLoc,
                                              const clang::Token& IncludeTok, llvm::StringRef FileName,
                                              bool IsAngled, clang::CharSourceRange FilenameRange,
                                              clang::OptionalFileEntryRef File,
                                              llvm::StringRef SearchPath, llvm::StringRef RelativePath,
                                              const clang::Module* SuggestedModule,
                                              bool ModuleImported,
                                              clang::SrcMgr::CharacteristicKind FileType)
    {
        if (!File) {
            return;
        }

        const std::string FE = File->getName().str();

        auto& SM = m_compiler.getSourceManager();
        if (!SM.isInMainFile(HashLoc)) {
            return;
        }

        std::string headerIncStr = IsAngled ? "<" + FileName.str() + ">"
                                            : "\"" + FileName.str() + "\"";
        m_includeStrMap[*File] = headerIncStr;
        m_includeStrSet.insert(headerIncStr);
	}
}