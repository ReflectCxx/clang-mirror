
#pragma once

#include <map>
#include <vector>
#include <string>
#include <unordered_map>

#include "Constants.h"

namespace clmr
{
    struct ASTObj {

        MetaKind metaKind;
        std::string header;
        std::string record;
        std::string function;
    };

    struct ASTFnSign
    {
        std::string returnType;
        std::string paramsType;
    };

    struct ASTCodeMeta
    {
        const ASTObj ast;
        std::vector<ASTFnSign> signatures;
    };

    struct ASTRecordMeta
    {
        using MemberFnsMap = std::unordered_map<std::string, clmr::ASTCodeMeta>;

        std::string record;
        MemberFnsMap methods;
	};
}