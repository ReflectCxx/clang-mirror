
#include <iostream>
#include <unordered_set>

#include "Logger.h"
#include "CLPPCallbacks.h"
#include "CLMirrorASTParser.h"
#include "CLMirrorASTVisitor.h"
#include "CLMirrorActionFactory.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/StaticAnalyzer/Frontend/AnalysisConsumer.h"

namespace {

	class CLMirrorASTConsumer : public clang::ASTConsumer
	{
		const std::string& m_currentSrcFile;

	public:

		CLMirrorASTConsumer(const std::string& pSrcFile)
			: m_currentSrcFile(pSrcFile)
		{ }

		virtual void HandleTranslationUnit(clang::ASTContext& Context)
		{
			clmr::CLMirrorASTVisitor visitor(m_currentSrcFile);
			visitor.TraverseDecl(Context.getTranslationUnitDecl());
		}
	};


	class CLMirrorFrontEndAction : public clang::ASTFrontendAction
	{
		std::string m_targetSrcFile;

	public:

		CLMirrorFrontEndAction() = default;

		std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& Compiler, llvm::StringRef InFile) override
		{
			Compiler.getDiagnosticOpts().ShowCarets = false;
			return std::make_unique<CLMirrorASTConsumer>(m_targetSrcFile);
		}

		bool BeginSourceFileAction(clang::CompilerInstance& CI) override {
			const auto& inputs = CI.getInvocation().getFrontendOpts().Inputs;
			m_targetSrcFile = inputs[0].getFile().str();

			auto& PP = CI.getPreprocessor();
			auto& SM = CI.getSourceManager();

			PP.addPPCallbacks(
				std::make_unique<clmr::CLPPCallbacks>(SM)
			);

			return true;
		}
	};
}


namespace clmr {

	std::unique_ptr<clang::FrontendAction> CLMirrorActionFactory::create()
	{
		return std::make_unique<CLMirrorFrontEndAction>();
	}
}