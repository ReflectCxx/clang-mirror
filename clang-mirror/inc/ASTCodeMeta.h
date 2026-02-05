
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
        std::string record;
        std::string function;
    };

    struct ASTFnSign
    {
        MetaKind metaKind;
        std::string returnType;
        std::string paramsType;
    };

    struct ASTCodeMeta
    {
        const bool isCtor;
        const ASTObj ast;
        std::vector<ASTFnSign> signatures;
    };

    struct ASTRecordMeta
    {
        std::string record;
        CxxFunctionsMap methods;
	};
}