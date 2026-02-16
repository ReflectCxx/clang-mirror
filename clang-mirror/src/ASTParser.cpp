

#include "ASTParser.h"
#include "ASTParserUtils.h"

#include <mutex>
#include <iostream>
#include <filesystem>

#include "Logger.h"
#include "Constants.h"
#include "ASTCodeManager.h"
#include "ClangActionFactory.h"

#include "clang/Basic/Diagnostic.h"
#include "clang-tidy/ClangTidy.h"

using namespace llvm;
using namespace clang;
using namespace clang::tidy;
using namespace clang::tooling;

namespace clmr
{
	ASTParser::ASTParser(const std::vector<std::string>& pFiles,
					     clang::tooling::CompilationDatabase& pCdb)
		: m_srcFiles(pFiles)
		, m_compileDb(pCdb)
	{ }


	bool ASTParser::parseFiles(const int pStartIndex, const int pEndIndex)
	{
		for (size_t index = pStartIndex; index <= pEndIndex; index++)
		{
			const auto& srcFilePath = m_srcFiles.at(index).c_str();

			Logger::outProgress(std::string(srcFilePath));

			if (!std::filesystem::exists(srcFilePath)) {
				Logger::outProgress(srcFilePath + std::string(". File not found..!"), false);
				continue;
			}

			llvm::IntrusiveRefCntPtr<vfs::OverlayFileSystem> baseFS = createBaseFS();
			if (!baseFS) {
				Logger::out("Failed to initialize vfs::OverlayFileSystem.");
				return false;
			}

			ClangTool clangTool(m_compileDb, { srcFilePath }, std::make_shared<PCHContainerOperations>(), baseFS);

			auto OwningOptionsProvider = createOptionsProvider(baseFS);

			ClangTidyContext context(std::move(OwningOptionsProvider), false, false);
			context.setEnableProfiling(false);

			ClangTidyDiagnosticConsumer diagConsumer(context);
			auto diagOpts = std::make_unique<DiagnosticOptions>();
			DiagnosticsEngine diagEngine(new DiagnosticIDs(), *diagOpts, &diagConsumer, false);
			
			context.setDiagnosticsEngine(std::move(diagOpts), &diagEngine);
			clangTool.setDiagnosticConsumer(&diagConsumer);

			auto actionFactory = std::make_unique<ClangActionFactory>();
			clangTool.run(actionFactory.get());

			const auto& errors = diagConsumer.take();
			if (errors.empty()) {
				ASTCodeManager::instance().emitRegistrationSource(srcFilePath, index);
			}
			else {
				ASTCodeManager::instance().compilationFailedFor(srcFilePath);
				return false;
			}
		}
		return true;
	}
}