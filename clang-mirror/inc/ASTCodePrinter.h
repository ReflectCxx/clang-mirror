#pragma once

#include <fstream>

#include "Constants.h"
#include "ASTCodeBuffer.h"

namespace clmr {
	struct ASTCodeMeta;
}

namespace clmr
{
	struct ASTCodePrint
	{
		static void outFunctionInitsDefs(const ASTCodeBuffer& pCb, std::ofstream& pOut);
		static void outTypeRecordInitsDefs(ASTCodeBuffer& pCb, std::ofstream& pOut);
		
		static void outFreeFnsDecls(std::ofstream& pOut, std::size_t pSrcIndex);
		static void outRecordInitDecls(std::ofstream& pOut, std::size_t pSrcIndex, std::size_t pTypeIndex);

		static void outRegisteredTypeRecordIDs(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut);
		static void outRegisteredFunctionIDs(const CxxFunctionsMap& pFunctionsMap, std::ofstream& pOut);
		static void outMethodIDsWithNamespaces(const std::string& pTypeID, const CxxFunctionsMap& pMethodsMap, std::ofstream& pOut);

	private:

		static std::string getFnIDsWithNameSpaces(const ASTCodeMeta& pMeta);
		static std::string getMethodIDDecleration(const std::string& pTypeID, const ASTCodeMeta& pMeta);
		
		static std::string getFnIDDeclaration(const ASTCodeMeta& pMeta);
		static std::string freeFunctionInitDefs(const ASTCodeMeta& pMeta);
		
		static std::string getTypeIDWithNamespaces(const std::string& pTypeID, const ASTCodeMeta& pMeta);
		static std::string recordTypeInitDefs(const ASTRecordMeta& pMeta);

		static void closeNS(std::string& pCodeStr, std::size_t pCount);
		static std::size_t openNS(std::string& pCodeStr, const std::string& pType);

		static std::string geSignaturesArrList(const std::vector<ASTFnSign>& pSigns);
		static std::string getMethodRegistrationExpr(const std::string& pRecordStr, const std::string& pFnID, 
			                                         const ASTCodeMeta& pCode);
	};
}