#pragma once

#include <fstream>

#include "Constants.h"

namespace clmr
{
	struct ASTCodePrinter
	{
		static void printRecordTypeIds(const CxxRecordsMap& pRecodsMap, std::fstream& pOut);
		static void printFreeFunctionIds(const CxxFunctionsMap& pFunctionsMap, std::fstream& pOut);
		static void printRegistrationDecls(const CxxRecordsMap& pRecodsMap, std::fstream& pOut);
	};
}