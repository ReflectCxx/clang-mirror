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
		static std::string printFnNamespace(const ASTCodeMeta& pMeta);
		static std::string printFnIdDeclarations(const ASTCodeMeta& pMeta);
		static std::string printTypeIdDeclaration(const ASTCodeMeta& pMeta);

		static void printTypeRecords(const CxxRecordsMap& pRecodsMap, std::fstream& pOut);
		static void printFreeFunctions(const CxxFunctionsMap& pFunctionsMap, std::fstream& pOut);
		static void printRegistrationDecls(const CxxRecordsMap& pRecodsMap, std::fstream& pOut);
	};
}