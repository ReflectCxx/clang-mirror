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

		static std::filesystem::path toRootDir(std::string_view pPath);
		static std::filesystem::path toClmrDir(std::string_view pPath);

		bool emitRegisteredIds(std::ofstream& pOut);
		bool emitRegistrationFns(std::ofstream& pOut);
		bool emitCxxMirrorHeader(std::ofstream& pOut);
		bool emitCxxMirrorSource(std::ofstream& pOut);
		bool emitRegistrationCpp(std::ofstream& pOut);

		using Emitter = bool(ASTCodeManager::*)(std::ofstream&);
		using GetDir = std::filesystem::path(*)(std::string_view);

		void dump(Emitter pEmiter, GetDir pGetDir, std::string_view pFile);

		ASTCodeManager();
		~ASTCodeManager();

	public:

		ASTCodeManager(ASTCodeManager&&) = delete;
		ASTCodeManager(const ASTCodeManager&) = delete;
		ASTCodeManager& operator=(ASTCodeManager&&) = delete;
		ASTCodeManager& operator=(const ASTCodeManager&) = delete;
		
		ASTCodeBuffer* getCodeBuffer(const std::string& pSrcFile, bool pCreate = false);

		static ASTCodeManager& instance();

		void emitCxxMirror();

		void setOutDir(const std::string& pOutDir);

		void compilationFailedFor(const std::string& pSrcFile);

		void emitRegistrationSource(const std::string& pSrcFile, std::size_t pIndex);
	};
}