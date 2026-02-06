#pragma once

#include "Constants.h"
#include "clang/Basic/Diagnostic.h"
#include "clang-tidy/ClangTidy.h"

namespace clmr {

	class ClangDiagnosticConsumer : public clang::tidy::ClangTidyDiagnosticConsumer
	{
		std::vector<ErrorTuple> m_errors;

	public:

		ClangDiagnosticConsumer(clang::tidy::ClangTidyContext& pContext);

		void HandleDiagnostic(clang::DiagnosticsEngine::Level pDiagLevel,const clang::Diagnostic& pInfo) override;

		const std::vector<ErrorTuple>& getCompilationErrors();
	};
}