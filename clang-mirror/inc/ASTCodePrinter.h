#pragma once

#include <fstream>

#include "Constants.h"

namespace clmr {
	struct ASTCodeMeta;
}

namespace clmr
{
	struct ASTCodePrinter
	{
		static std::string getIncludesForRegistrations();

		static void printTypeRecords(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);
		static void printFreeFunctions(const CxxFunctionsMap& pFunctionsMap, std::ofstream& pOut);
		static void printMemberFunctions(const CxxFunctionsMap& pMethodsMap, std::ofstream& pOut);

		static void printRegistrationDecls(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);
		static void printRegistrationDecls(const CxxFunctionsMap& pFnsMap, std::ofstream& pOut);
		static void printRegistrationDefns(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);
		static void printRegistrationDefns(const CxxFunctionsMap& pFnsMap, std::ofstream& pOut);

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