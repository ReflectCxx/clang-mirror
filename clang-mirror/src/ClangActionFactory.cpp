
#include <iostream>
#include <unordered_set>

#include "Logger.h"
#include "ASTParser.h"
#include "ClangPPCallbacks.h"
#include "ClangASTVisitor.h"
#include "ClangActionFactory.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/StaticAnalyzer/Frontend/AnalysisConsumer.h"

namespace {

	class ClangASTConsumer : public clang::ASTConsumer
	{
		const std::string m_currentSrcFile;
		clmr::ClangPPCallbacks& m_preProcessor;

	public:

		ClangASTConsumer(const std::string& pSrcFile, clmr::ClangPPCallbacks& pPP)
			: m_currentSrcFile(pSrcFile)
			, m_preProcessor(pPP)
		{ }

		void HandleTranslationUnit(clang::ASTContext& Context) override
		{
			if (Context.getDiagnostics().hasErrorOccurred()) {
				return;
			}

			clmr::ClangASTVisitor visitor(m_currentSrcFile, m_preProcessor);
			visitor.TraverseDecl(Context.getTranslationUnitDecl());
		}
	};


	class CLMirrorFrontEndAction : public clang::ASTFrontendAction
	{
		clmr::ClangActionFactory* m_actionFactory = nullptr;

	public:

		void setActionFactory(clmr::ClangActionFactory* pActionFactory) {
			m_actionFactory = pActionFactory;
		}

		std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& CI, llvm::StringRef InFile) override
		{
			auto PPCb = std::make_unique<clmr::ClangPPCallbacks>(CI);
			auto* ptr = PPCb.get();

			m_actionFactory->setTargetSrcFile(InFile.str());

			CI.getPreprocessor().addPPCallbacks(std::move(PPCb));
			return std::make_unique<ClangASTConsumer>(InFile.str(), *ptr);
		}
	};
}


namespace clmr {

	void ClangActionFactory::setTargetSrcFile(const std::string pTargetSrcFile) {
		m_targetSrcFile = pTargetSrcFile;
	}

	std::unique_ptr<clang::FrontendAction> ClangActionFactory::create()
	{   
		auto actionFactory = std::make_unique<CLMirrorFrontEndAction>();
		actionFactory->setActionFactory(this);
		return std::move(actionFactory);
	}
}