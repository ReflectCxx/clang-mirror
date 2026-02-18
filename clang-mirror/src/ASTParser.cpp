
#include "ASTParser.h"

#include "Logger.h"
#include "ASTCodeManager.h"
#include "ClangActionFactory.h"

#include "clang/Basic/Diagnostic.h"
#include "clang-tidy/ClangTidy.h"

using namespace llvm;
using namespace clang;
using namespace clang::tidy;
using namespace clang::tooling;

namespace 
{
    static std::unique_ptr<clang::tidy::ClangTidyOptionsProvider> createOptionsProvider()
    {
        clang::tidy::ClangTidyOptions defaultOptions;
        clang::tidy::ClangTidyOptions overrideOptions;
        clang::tidy::ClangTidyGlobalOptions globalOptions;

        return std::make_unique<clang::tidy::FileOptionsProvider>(
			std::move(globalOptions), 
			std::move(defaultOptions),
			std::move(overrideOptions)
		);
	}
}

namespace clmr
{
	ASTParser::ASTParser(const std::vector<std::string>& pFiles)
		: m_srcFiles(pFiles)
	{ }


	bool ASTParser::parseFiles(clang::tooling::CompilationDatabase& pCdb, const int pStartIndex, const int pEndIndex)
	{
		bool anyRegistrationSrcEmitted = false;
		for (size_t index = pStartIndex; index <= pEndIndex; index++)
		{
			const auto& srcFilePath = m_srcFiles.at(index).c_str();

			Logger::outProgress(std::string(srcFilePath));

			if (!std::filesystem::exists(srcFilePath)) {
				Logger::outProgress(srcFilePath + std::string(". File not found..!"), false);
				continue;
			}

			ClangTool clangTool(pCdb, { srcFilePath }, std::make_shared<PCHContainerOperations>());

			ClangTidyContext context(createOptionsProvider(), false, false);
			context.setEnableProfiling(false);

			ClangTidyDiagnosticConsumer diagConsumer(context);
			auto diagOpts = std::make_unique<DiagnosticOptions>();
			DiagnosticsEngine diagEngine(new DiagnosticIDs(), *diagOpts, &diagConsumer, false);
			
			context.setDiagnosticsEngine(std::move(diagOpts), &diagEngine);
			clangTool.setDiagnosticConsumer(&diagConsumer);

			auto actionFactory = std::make_unique<ClangActionFactory>();
			clangTool.run(actionFactory.get());

			bool foundErrors = llvm::any_of(diagConsumer.take(), [](const ClangTidyError& E) {
				return E.DiagLevel == ClangTidyError::Error;
			});

			if (!foundErrors) {
				ASTCodeManager::instance().emitRegistrationSource(srcFilePath, index);
				anyRegistrationSrcEmitted = true;
			}
			else {
				ASTCodeManager::instance().compilationFailedFor(srcFilePath);
			}
		}
		return anyRegistrationSrcEmitted;
	}
}