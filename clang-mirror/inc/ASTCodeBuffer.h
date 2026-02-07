#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "ASTCodeMeta.h"

namespace clmr
{
	class ASTCodeBuffer
	{
		bool m_errorsFound;
		const std::string m_srcFile;
		std::size_t m_SrcFileIndex;
		CxxRecordsMap m_recordsMap;
		CxxFunctionsMap m_freeFnsMap;

		using IncSet = std::unordered_set<std::string>;
		std::unordered_set<std::string> m_incFiles;

		static ASTRecordMeta& getRecordCodeMeta(CxxRecordsMap& pFnMetaMap, const std::string& pTypeStr);

		static ASTCodeMeta& addFunctionCodeMeta(CxxFunctionsMap& pFnMetaMap, const ASTCodeMeta& pFnMeta);

	public:

		ASTCodeBuffer(ASTCodeBuffer&&) = default;
		ASTCodeBuffer(const ASTCodeBuffer&) = default;
		ASTCodeBuffer& operator=(ASTCodeBuffer&&) = delete;
		ASTCodeBuffer& operator=(const ASTCodeBuffer&) = delete;

		ASTCodeBuffer(const std::string& pSrcFile);

		GETTER_BOOL(CompilationFailed, m_errorsFound)
		GETTER_CREF(IncSet, IncludesSet, m_incFiles)
		GETTER_CREF(std::string, SrcFile, m_srcFile)
		GETTER_CREF(CxxRecordsMap, RecordsMap, m_recordsMap)
		GETTER_CREF(CxxFunctionsMap, FreeFunctionsMap, m_freeFnsMap)

		void setErrorsFound(bool pErrorFound);
		void addFunction(MetaKind pMK, const ASTObj& pAst, const std::string& pRecord,
			             const std::string& pReturn, const std::string& pParams);
	};
}