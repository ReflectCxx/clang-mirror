#pragma once

#include <fstream>

#include "Constants.h"

namespace clmirror
{
	struct ASTCodePrinter
	{
		static void printRecordTypeIds(const RtlRecordsMap& pRecodsMap, std::fstream& pOut);
		static void printFreeFunctionIds(const RtlFunctionsMap& pFunctionsMap, std::fstream& pOut);
		static void printRegistrationDecls(const RtlRecordsMap& pRecodsMap, std::fstream& pOut);
	};
}