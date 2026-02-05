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
		using CodeBuffsT = std::unordered_map<std::string, std::unique_ptr<ASTCodeBuffer>>;

		std::string m_outPath;
		CodeBuffsT m_codeGens;

		static std::filesystem::path toRootDir(std::string_view pPath);
		static std::filesystem::path toClmrDir(std::string_view pPath);

		void emitRegisteredIds(std::ofstream& pOut, ASTCodeBuffer*);
		void emitRegistrationFns(std::ofstream& pOut, ASTCodeBuffer*);
		void emitCxxMirrorHeader(std::ofstream& pOut, ASTCodeBuffer*);
		void emitCxxMirrorSource(std::ofstream& pOut, ASTCodeBuffer*);
		void emitRegistrationCpp(std::ofstream& pOut, ASTCodeBuffer*);

		using Emitter = void(*)(std::ofstream&, ASTCodeBuffer*);
		using GetDir = std::filesystem::path(*)(std::string_view);

		void dump(Emitter pEmiter, GetDir pGetDir, std::string_view pFile, ASTCodeBuffer* pCodeBff = nullptr);

		ASTCodeManager() = default;

	public:

		ASTCodeManager(ASTCodeManager&&) = delete;
		ASTCodeManager(const ASTCodeManager&) = delete;
		ASTCodeManager& operator=(ASTCodeManager&&) = delete;
		ASTCodeManager& operator=(const ASTCodeManager&) = delete;
		
		GETTER_CREF(CodeBuffsT, CodeBuffers, m_codeGens)

		ASTCodeBuffer* getCodeBuffer(const std::string& pSrcFile, bool pCreate = false);

		static ASTCodeManager& instance();

		void emitCxxMirror();

		void setOutDir(const std::string& pOutDir);

		void compilationFailedFor(const std::string& pSrcFile);

		void emitRegistrationSource(const std::string& pSrcFile, std::size_t pIndex);
	};
}