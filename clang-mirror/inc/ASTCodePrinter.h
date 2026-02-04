#pragma once

#include <fstream>

#include "Constants.h"

namespace clmr {
	struct ASTCodeMeta;
}

namespace clmr
{
	struct ASTCodePrint
	{
		static void outRecordInitDefs(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);
		static void outFreeFnsInitDefs(const CxxFunctionsMap& pFnsMap, std::ofstream& pOut);
		
		static void outFreeFnsDecls(const CxxFunctionsMap& pFnsMap, std::ofstream& pOut);
		static void outRecordInitDecls(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);

		static void outTypeRecordIDs(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);
		static void outFreeFunctionIDs(const CxxFunctionsMap& pFunctionsMap, std::ofstream& pOut);
		static void outMemberFunctionIDs(const CxxFunctionsMap& pMethodsMap, std::ofstream& pOut);

	private:

		static std::string printNamespaceFnIDs(const ASTCodeMeta& pMeta);
		static std::string printRecordNamespaceFnIDs(const ASTCodeMeta& pMeta);
		
		static std::string printFreeFnsIDs(const ASTCodeMeta& pMeta);
		static std::string printFreeFnsInitDefs(const std::string& pFnName);
		static std::string printFreeFnsInitDecls(const std::string& pFnName);

		static std::string printTypeRecordIDs(const ASTCodeMeta& pMeta);
		static std::string printTypeRecordInitDefs(const std::string& pRecordName);
		static std::string printTypeRecordInitDecls(const std::string& pRecordName);
	};
}