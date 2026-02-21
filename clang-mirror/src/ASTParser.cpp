
#include "ASTParser.h"

#include "Logger.h"
#include "ASTCodeManager.h"
#include "ClangActionFactory.h"

#include "clang/Basic/Diagnostic.h"
#include "clang-tidy/ClangTidy.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

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
			const auto& cmdSrcFilePath = m_srcFiles.at(index).c_str();
			Logger::outProgress(std::string(cmdSrcFilePath));
			
            auto diagOpts = std::make_unique<clang::DiagnosticOptions>();
			diagOpts->ShowColors = true;
			diagOpts->ShowCarets = true;
			diagOpts->ShowFixits = true;
			diagOpts->ShowColumn = true;
			diagOpts->ShowSourceRanges = true;
			diagOpts->ShowOptionNames = true;

			ClangTool clangTool(pCdb, { cmdSrcFilePath }, std::make_shared<PCHContainerOperations>());
			auto diagConsumer = std::make_unique<clang::TextDiagnosticPrinter>(llvm::errs(), *diagOpts);
			auto actionFactory = std::make_unique<ClangActionFactory>();
			
			clangTool.setDiagnosticConsumer(diagConsumer.get());
			auto result = clangTool.run(actionFactory.get());

			auto& clangSrcFilePath = actionFactory->getTargetSrcFile();
			if (result == 0 && ASTCodeManager::instance().emitRegistrationSource(clangSrcFilePath, index)) {
				anyRegistrationSrcEmitted = true;
			}
			else {
				ASTCodeManager::instance().compilationFailedFor(clangSrcFilePath);
			}
		}
		return anyRegistrationSrcEmitted;
	}
}