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
		static void outRegistrationDefs(const CxxFunctionsMap& pFnsMap, std::ofstream& pOut);
		static void outRegistrationDefs(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);
		static void outRegistrationDecls(const CxxFunctionsMap& pFnsMap, std::ofstream& pOut);
		static void outRegistrationDecls(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);

		static void outTypeRecordIDs(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);
		static void outFreeFunctionIDs(const CxxFunctionsMap& pFunctionsMap, std::ofstream& pOut);
		static void outMemberFunctionIDs(const CxxFunctionsMap& pMethodsMap, std::ofstream& pOut);

	private:

		static std::string printFnNamespaceFnIDs(const ASTCodeMeta& pMeta);
		static std::string printMethodNamespaceFnIDs(const ASTCodeMeta& pMeta);
		
		static std::string printFnIdDeclarations(const ASTCodeMeta& pMeta);
		static std::string printFnInitDefinitions(const std::string& pFnName);
		static std::string printFnInitDeclarations(const std::string& pFnName);

		static std::string printTypeIdDeclaration(const ASTCodeMeta& pMeta);
		static std::string printTypeInitDeclarations(const std::string& pRecordName);
	};
}