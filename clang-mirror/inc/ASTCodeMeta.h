
#pragma once

#include <map>
#include <vector>
#include <string>
#include <unordered_map>

#include "Constants.h"

namespace clmr
{
    struct ASTObj {

        std::string header;
        std::string function;
    };

    struct ASTFnSign
    {
        MetaKind metaKind;
        std::string returnStr;
        std::string paramsStr;
    };

    struct ASTCodeMeta
    {
        const bool isCtor;
        const ASTObj ast;
        std::vector<ASTFnSign> signatures;
    };

    struct ASTRecordMeta
    {
        std::size_t typeIndex;
        std::string recordStr;
        CxxFunctionsMap methods;
	};
}