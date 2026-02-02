
#include "CLMirrorDiagnosticConsumer.h"
#include "Logger.h"
#include "clang/Basic/SourceManager.h"

using namespace clang;
using namespace clang::tidy;

namespace clmr
{
    CLMirrorDiagnosticConsumer::CLMirrorDiagnosticConsumer(ClangTidyContext& pContext)
        : clang::tidy::ClangTidyDiagnosticConsumer(pContext)
    { }

    const std::vector<ErrorTuple>& CLMirrorDiagnosticConsumer::getCompilationErrors() {
        return m_errors;
    }

    void CLMirrorDiagnosticConsumer::HandleDiagnostic(DiagnosticsEngine::Level pDiagLevel, const Diagnostic& pInfo)
    {
        if (pInfo.hasSourceManager() && pInfo.getLocation().isValid())
        {
            const SourceManager& srcManager = pInfo.getSourceManager();
            if (!srcManager.isInSystemHeader(pInfo.getLocation()) && !srcManager.isInSystemMacro(pInfo.getLocation()))
            {
                if (pDiagLevel == DiagnosticsEngine::Level::Error || pDiagLevel == DiagnosticsEngine::Level::Fatal)
                {
                    const SourceLocation& errLoc = pInfo.getLocation();
                    const SourceManager& srcManager = pInfo.getSourceManager();
                    StringRef fileName = srcManager.getFilename(errLoc);
                    unsigned lineNumber = srcManager.getSpellingLineNumber(errLoc);
                    unsigned colmNumber = srcManager.getSpellingColumnNumber(errLoc);
                    SmallString<128> msgStr;
                    pInfo.FormatDiagnostic(msgStr);
                    const std::string& lineInfo = "(" + std::to_string(lineNumber) + "," + std::to_string(colmNumber) + ")";
                    m_errors.push_back(std::make_tuple(fileName.str(), lineInfo, msgStr.str().str()));
				}
			}
		}
	}
}