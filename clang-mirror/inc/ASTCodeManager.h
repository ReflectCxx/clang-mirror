#pragma once

#include <string>
#include <optional>
#include <filesystem>
#include <unordered_map>

namespace clmirror {
	class ASTCodeGenerator;
}

namespace clmirror 
{
	class ASTCodeManager
	{
		std::string m_outPath;
		std::unordered_map<std::string, ASTCodeGenerator*> m_codeGens;

		std::filesystem::path getOutDir();

		void dumpMetadataIds(std::fstream& pOut);
		void dumpRegistrationDecls(std::fstream& pOut);

		static void printRecordTypeIds(const RtlRecordsMap& pRecodsMap, std::fstream& pOut);
		static void printFreeFunctionIds(const RtlFunctionsMap& pFunctionsMap, std::fstream& pOut);
		static void printRegistrationDecls(const RtlRecordsMap& pRecodsMap, std::fstream& pOut);

		ASTCodeManager();
		~ASTCodeManager();

	public:

		ASTCodeManager(ASTCodeManager&&) = delete;
		ASTCodeManager(const ASTCodeManager&) = delete;
		ASTCodeManager& operator=(ASTCodeManager&&) = delete;
		ASTCodeManager& operator=(const ASTCodeManager&) = delete;
		
		ASTCodeGenerator* getCodeGenerator(const std::string& pSrcFile, bool pCreate = false);

		static ASTCodeManager& instance();

		void dumpCxxMirror();

		void setOutDir(const std::string& pOutDir);

		void dumpRegistrations(const std::string& pSrcFile, std::size_t pIndex);
	};
}