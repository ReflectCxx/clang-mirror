#pragma once

#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang-tidy/ClangTidyDiagnosticConsumer.h"


namespace clmr 
{
	class ClangActionFactory : public clang::tooling::FrontendActionFactory
	{
	public:
		std::unique_ptr<clang::FrontendAction> create() override;
	};
}