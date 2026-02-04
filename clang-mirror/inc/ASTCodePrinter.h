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
		static std::string printFreeFnsInitDefs(const ASTCodeMeta& pMeta);
		static std::string printFreeFnsInitDecls(const std::string& pFnName);
		
		static std::string printRecordIDs(const ASTCodeMeta& pMeta);
		static std::string printRecordInitDefs(const ASTRecordMeta& pMeta);
		static std::string printRecordInitDecls(const std::string& pRecord);

		static void closeNS(std::string& pCodeStr, std::size_t pCount);
		static std::size_t openNS(std::string& pCodeStr, const std::string& pType);
		static std::string getSignaturesJSON(const std::vector<ASTFnSign>& pSigns);
	};
}