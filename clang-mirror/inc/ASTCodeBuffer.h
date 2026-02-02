#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "Constants.h"
#include "ASTCodeMeta.h"

namespace clmirror 
{
	class ASTCodeManager;
}

namespace clmirror
{
	class ASTCodeBuffer
	{
		bool m_errorsFound;
		const std::string m_srcFile;
		RtlRecordsMap m_recordsMap;
		RtlFunctionsMap m_freeFnsMap;
		std::unordered_set<std::string> m_incFiles;

		ASTCodeBuffer(const std::string& pSrcFile);

		static ASTMetaType& getRecordCodeMeta(RtlRecordsMap& pFnMetaMap, const std::string& pTypeStr);

		static ASTCodeMeta& addFunctionCodeMeta(RtlFunctionsMap& pFnMetaMap, const ASTCodeMeta& pFnMeta);

	public:

		ASTCodeBuffer(ASTCodeBuffer&&) = default;
		ASTCodeBuffer(const ASTCodeBuffer&) = default;
		ASTCodeBuffer& operator=(ASTCodeBuffer&&) = delete;
		ASTCodeBuffer& operator=(const ASTCodeBuffer&) = delete;

		GETTER_BOOL(CompilationFailed, m_errorsFound)
		GETTER_CREF(std::string, SrcFile, m_srcFile)
		GETTER_CREF(RtlRecordsMap, RecordsMap, m_recordsMap)
		GETTER_CREF(RtlFunctionsMap, FreeFunctionsMap, m_freeFnsMap)

		void addFunction(MetaKind pMetaKind, const std::string& pHeaderFile, const std::string& pRecord,
						 const std::string& pFnName, const std::string& pParamTypes);

		friend ASTCodeManager;
	};
}