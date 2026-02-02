#pragma once

#include <vector>
#include <string>

namespace clang::tooling {
	class CompilationDatabase;
}

namespace cxx {
	class ASTCodeBuffer;
}

namespace cxx
{
	class ASTParser
	{
		const std::vector<std::string>& m_srcFiles;
		
		clang::tooling::CompilationDatabase& m_compileDb;

		ASTParser() = delete;

	public:

		ASTParser(const std::vector<std::string>& pFiles, clang::tooling::CompilationDatabase& pCdb);

		const int parseFiles(const int pStartIndex, const int pEndIndex);
	};
}