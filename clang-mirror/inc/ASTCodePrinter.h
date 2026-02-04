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
		static void outTypeRecords(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);
		static void outFreeFunctions(const CxxFunctionsMap& pFunctionsMap, std::ofstream& pOut);
		static void outMemberFunctions(const CxxFunctionsMap& pMethodsMap, std::ofstream& pOut);

		static void outRegistrationDecls(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);
		static void outRegistrationDecls(const CxxFunctionsMap& pFnsMap, std::ofstream& pOut);
		static void outRegistrationDefns(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);
		static void outRegistrationDefns(const CxxFunctionsMap& pFnsMap, std::ofstream& pOut);

	private:

		static std::string printFnNamespace(const ASTCodeMeta& pMeta);
		static std::string printMethodNamespace(const ASTCodeMeta& pMeta);
		
		static std::string printFnIdDeclarations(const ASTCodeMeta& pMeta);
		static std::string printFnInitDefinitions(const std::string& pFnName);
		static std::string printFnInitDeclarations(const std::string& pFnName);

		static std::string printTypeIdDeclaration(const ASTCodeMeta& pMeta);
		static std::string printTypeInitDeclarations(const std::string& pRecordName);
	};
}