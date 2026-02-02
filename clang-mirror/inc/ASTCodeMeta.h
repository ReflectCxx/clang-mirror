
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
        const ASTObj m_astObj;
        std::vector<std::string> m_signaturesTy;
        
        constexpr const ASTObj& ast() const;
        std::string toSignatureSyntax() const;
        std::string toRegistrationDeclSyntax() const;
        std::string toRecordIdentifierSyntax() const;
        std::string toMethodIdentifierSyntax() const;
        std::string toFunctionIdentifierSyntax() const;
    };

    struct ASTMetaType
    {
        using MemberFnsMap = std::unordered_map<std::string, clmr::ASTCodeMeta>;
        
        std::string typeStr;
        MemberFnsMap methods;
	};
}