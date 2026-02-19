#pragma once

#include "Constants.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang-tidy/ClangTidyDiagnosticConsumer.h"


namespace clmr 
{
	class ClangActionFactory : public clang::tooling::FrontendActionFactory
	{
		std::string m_targetSrcFile;

	public:

		GETTER_CREF(std::string, TargetSrcFile, m_targetSrcFile)

		void setTargetSrcFile(const std::string pTargetSrcFile);

		std::unique_ptr<clang::FrontendAction> create() override;
	};
}