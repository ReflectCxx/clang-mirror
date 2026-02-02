#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "Constants.h"
#include "ASTCodeMeta.h"

namespace clmr 
{
	class ASTCodeManager;
}

namespace clmr
{
	class ASTCodeBuffer
	{
		bool m_errorsFound;
		const std::string m_srcFile;
		CxxRecordsMap m_recordsMap;
		CxxFunctionsMap m_freeFnsMap;
		std::unordered_set<std::string> m_incFiles;

		ASTCodeBuffer(const std::string& pSrcFile);

		static ASTMetaType& getRecordCodeMeta(CxxRecordsMap& pFnMetaMap, const std::string& pTypeStr);

		static ASTCodeMeta& addFunctionCodeMeta(CxxFunctionsMap& pFnMetaMap, const ASTCodeMeta& pFnMeta);

	public:

		ASTCodeBuffer(ASTCodeBuffer&&) = default;
		ASTCodeBuffer(const ASTCodeBuffer&) = default;
		ASTCodeBuffer& operator=(ASTCodeBuffer&&) = delete;
		ASTCodeBuffer& operator=(const ASTCodeBuffer&) = delete;

		GETTER_BOOL(CompilationFailed, m_errorsFound)
		GETTER_CREF(std::string, SrcFile, m_srcFile)
		GETTER_CREF(CxxRecordsMap, RecordsMap, m_recordsMap)
		GETTER_CREF(CxxFunctionsMap, FreeFunctionsMap, m_freeFnsMap)

		void addFunction(const ASTObj& pAst, const std::string& pReturn, const std::string& pParams);

		friend ASTCodeManager;
	};
}