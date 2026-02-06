
#include <iostream>
#include <unordered_set>

#include "Logger.h"
#include "ASTParser.h"
#include "ClangPPCallbacks.h"
#include "ClangASTVisitor.h"
#include "ClangActionFactory.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/StaticAnalyzer/Frontend/AnalysisConsumer.h"

namespace {

	class ClangASTConsumer : public clang::ASTConsumer
	{
		
		const std::string& m_currentSrcFile;
		clmr::ClangPPCallbacks& m_preProcessor;

	public:

		ClangASTConsumer(const std::string& pSrcFile, clmr::ClangPPCallbacks& pPP)
			: m_currentSrcFile(pSrcFile)
			, m_preProcessor(pPP)
		{ }

		void HandleTranslationUnit(clang::ASTContext& Context) override
		{
			clmr::ClangASTVisitor visitor(m_currentSrcFile, m_preProcessor);
			visitor.TraverseDecl(Context.getTranslationUnitDecl());
		}
	};


	class CLMirrorFrontEndAction : public clang::ASTFrontendAction
	{
		std::string m_targetSrcFile;
		clmr::ClangPPCallbacks* m_preProcessor = nullptr;

	public:

		CLMirrorFrontEndAction() = default;

		// This is always called after `BeginSourceFileAction`
		std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& Compiler, llvm::StringRef InFile) override
		{
			Compiler.getDiagnosticOpts().ShowCarets = false;
			return std::make_unique<ClangASTConsumer>(m_targetSrcFile, *m_preProcessor);
		}

		bool BeginSourceFileAction(clang::CompilerInstance& CI) override 
		{
			const auto& inputs = CI.getInvocation().getFrontendOpts().Inputs;
			m_targetSrcFile = inputs[0].getFile().str();

			auto& PP = CI.getPreprocessor();
			auto& SM = CI.getSourceManager();
			auto PPCb = std::make_unique<clmr::ClangPPCallbacks>(SM);

			m_preProcessor = PPCb.get();
			PP.addPPCallbacks(std::move(PPCb));

			return true;
		}
	};
}


namespace clmr {

	std::unique_ptr<clang::FrontendAction> ClangActionFactory::create()
	{
		return std::make_unique<CLMirrorFrontEndAction>();
	}
}