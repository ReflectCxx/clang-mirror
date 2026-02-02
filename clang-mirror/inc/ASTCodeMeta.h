
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

    struct ASTCodeMeta
    {
        const ASTObj ast;
        std::vector<std::string> signatures;

        std::string toRegistrationDeclSyntax() const;
        std::string toMethodIdentifierSyntax() const;
    };

    struct ASTMetaType
    {
        using MemberFnsMap = std::unordered_map<std::string, clmr::ASTCodeMeta>;
        
        std::string typeStr;
        MemberFnsMap methods;
	};
}