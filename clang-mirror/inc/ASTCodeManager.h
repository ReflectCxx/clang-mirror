#pragma once

#include <string>
#include <optional>
#include <filesystem>
#include <unordered_map>

namespace clmr {
	class ASTCodeBuffer;
}

namespace clmr 
{
	class ASTCodeManager
	{
		std::string m_outPath;
		std::unordered_map<std::string, ASTCodeBuffer*> m_codeGens;

		std::filesystem::path getOutDir();

		void dumpMetadataIds(std::fstream& pOut);
		void dumpRegistrationDecls(std::fstream& pOut);

		ASTCodeManager();
		~ASTCodeManager();

	public:

		ASTCodeManager(ASTCodeManager&&) = delete;
		ASTCodeManager(const ASTCodeManager&) = delete;
		ASTCodeManager& operator=(ASTCodeManager&&) = delete;
		ASTCodeManager& operator=(const ASTCodeManager&) = delete;
		
		ASTCodeBuffer* getCodeBuffer(const std::string& pSrcFile, bool pCreate = false);

		static ASTCodeManager& instance();

		void createCxxMirror();

		void setOutDir(const std::string& pOutDir);

		void compilationFailedFor(const std::string& pSrcFile);

		void dumpRegistrations(const std::string& pSrcFile, std::size_t pIndex);
	};
}