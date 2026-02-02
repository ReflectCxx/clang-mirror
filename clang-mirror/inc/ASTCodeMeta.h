
#pragma once

#include <map>
#include <vector>
#include <string>
#include <unordered_map>

#include "Constants.h"

namespace clmirror
{
    struct ASTCodeMeta
    {
        const MetaKind m_metaKind;
		
        const std::string m_header;
        const std::string m_record;
        const std::string m_function;

        std::vector<std::string> m_argTypes;
        
        std::string toSignatureSyntax() const;
        std::string toRegistrationDeclSyntax() const;
        std::string toRecordIdentifierSyntax() const;
        std::string toMethodIdentifierSyntax() const;
        std::string toFunctionIdentifierSyntax() const;
    };

    struct ASTMetaType
    {
        using MemberFnsMap = std::unordered_map<std::string, clmirror::ASTCodeMeta>;
        
        std::string typeStr;
        MemberFnsMap methods;
	};
}