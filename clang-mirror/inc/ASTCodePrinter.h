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
		static void outMemberFunctionIDs(const std::string& pRecord, const CxxFunctionsMap& pMethodsMap, std::ofstream& pOut);

	private:

		static std::string freeFunctionsNsIDs(const ASTCodeMeta& pMeta);
		static std::string memberFunctionsNsIDs(const std::string& pRecord, const ASTCodeMeta& pMeta);
		
		static std::string freeFunctionIDs(const ASTCodeMeta& pMeta);
		static std::string freeFunctionInitDefs(const ASTCodeMeta& pMeta);
		static std::string freeFunctionInitDecls(const std::string& pFnName);
		
		static std::string recordTypeIDs(const std::string& pRecord, const ASTCodeMeta& pMeta);
		static std::string recordTypeInitDefs(const ASTRecordMeta& pMeta);
		static std::string recordTypeInitDecls(const std::string& pRecord);

		static void closeNS(std::string& pCodeStr, std::size_t pCount);
		static std::size_t openNS(std::string& pCodeStr, const std::string& pType);
		static std::string getSignaturesJSON(const std::vector<ASTFnSign>& pSigns);
		static std::string getMethodRegistrationExpr(const std::string& pRecordStr, const std::string& pFnID, 
			                                         const ASTCodeMeta& pCode);
	};
}