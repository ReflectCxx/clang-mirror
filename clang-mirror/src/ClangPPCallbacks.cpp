
#include "ClangPPCallbacks.h"

namespace clmr {

    ClangPPCallbacks::ClangPPCallbacks(clang::SourceManager& SM, clang::CompilerInstance& CI)
        : m_srcMgr(SM)
        , m_compiler(CI)
    { }

    void ClangPPCallbacks::InclusionDirective(clang::SourceLocation pHashLoc,
                                              const clang::Token& pIncludeTok, llvm::StringRef pFileName,
                                              bool pIsAngled, clang::CharSourceRange pFilenameRange,
                                              clang::OptionalFileEntryRef pFile,
                                              llvm::StringRef pSearchPath, llvm::StringRef pRelativePath,
                                              const clang::Module* pSuggestedModule,
                                              bool pModuleImported,
                                              clang::SrcMgr::CharacteristicKind pFileType)
    {
        if (!pFile) {
            return;
        }

        auto& SM = m_compiler.getSourceManager();
        if (!SM.isInMainFile(pHashLoc)) {
            return;
        }

        std::string headerIncStr = pIsAngled ? "<" + pFileName.str() + ">"
                                             : "\"" + pFileName.str() + "\"";
        m_includeStrMap[*pFile] = headerIncStr;
        m_includeStrSet.insert(headerIncStr);
	}
}