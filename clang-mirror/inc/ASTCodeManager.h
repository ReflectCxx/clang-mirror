#pragma once

#include <string>
#include <memory>
#include <optional>
#include <filesystem>
#include <unordered_map>

#include "Constants.h"

namespace clmr {
	class ASTCodeBuffer;
}

namespace clmr 
{
	class ASTCodeManager
	{
		using CodeBuffMap = std::unordered_map<std::string, std::unique_ptr<ASTCodeBuffer>>;

		std::string m_outPath;
		CodeBuffMap m_codeBuffs;

		static std::filesystem::path toRootDir(std::string_view pPath);
		static std::filesystem::path toSrcDir(std::string_view pPath);

		using Emitter = void(*)(std::ofstream&, ASTCodeBuffer*);
		using GetDir = std::filesystem::path(*)(std::string_view);

		bool dump(std::string_view pFile, GetDir pGetDir, 
			      Emitter pEmiter, ASTCodeBuffer* pCodeBff = nullptr);

		ASTCodeManager() = default;

	public:

		ASTCodeManager(ASTCodeManager&&) = delete;
		ASTCodeManager(const ASTCodeManager&) = delete;
		ASTCodeManager& operator=(ASTCodeManager&&) = delete;
		ASTCodeManager& operator=(const ASTCodeManager&) = delete;
		
		GETTER_CREF(CodeBuffMap, CodeBufferMap, m_codeBuffs)

		ASTCodeBuffer* getCodeBuffer(const std::string& pSrcFile, bool pCreate = false);

		static ASTCodeManager& instance();

		bool emitCxxMirror();

		void setOutDir(const std::string& pOutDir);

		void compilationFailedFor(const std::string& pSrcFile);

		bool emitRegistrationSource(const std::string& pSrcFile, std::size_t pIndex);
	};
}