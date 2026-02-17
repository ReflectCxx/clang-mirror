#pragma once

#include <vector>
#include <string>

namespace clang::tooling {
	class CompilationDatabase;
}

namespace clmr {
	class ASTCodeBuffer;
}

namespace clmr
{
	class ASTParser
	{
		const std::vector<std::string>& m_srcFiles;

		ASTParser() = delete;

	public:

		ASTParser(const std::vector<std::string>& pFiles);

		bool parseFiles(clang::tooling::CompilationDatabase& pCdb, const int pStartIndex, const int pEndIndex);
	};
}