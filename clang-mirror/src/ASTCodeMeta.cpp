
#include <map>
#include <vector>
#include <string>

#include "Constants.h"
#include "StringUtils.h"
#include "ASTCodeMeta.h"

namespace cxx
{
    constexpr const ASTObj& ASTCodeMeta::ast() const
    {
        return m_astObj;
    }

    std::string ASTCodeMeta::toSignatureSyntax() const
    {
        return std::string("std::string(void)");
    }

    std::string ASTCodeMeta::toFunctionIdentifierSyntax() const
    {
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(ast().function);
        std::string fnName = typenames.back();
        typenames.pop_back();

        std::string syntaxStr = "\nnamespace " + std::string(NS_FUNCTION) + " {";
        for (const auto& typeStr : typenames) {
            syntaxStr.append("\nnamespace " + typeStr + " {");
        }

        syntaxStr.append("\nnamespace " + fnName + " {")
                 .append("\n    inline constexpr std::string_view id = \"")
                 .append(ast().function)
                 .append("\";");

        for (std::size_t i = 0; i < m_signaturesTy.size(); i++) {
            syntaxStr.append("\n    inline constexpr std::string_view sign" + std::to_string(i) + " = \"")
                     .append(m_signaturesTy[i])
                     .append("\";");
        }
        syntaxStr.append("\n}");

        for (auto& _ : typenames) {
            syntaxStr.append("}");
        }
        syntaxStr.append("}");
        return syntaxStr;
    }


    std::string ASTCodeMeta::toMethodIdentifierSyntax() const
    {
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(ast().record);

        std::string syntaxStr = "\nnamespace " + std::string(NS_TYPE) + " {";
        for (const auto& typeStr : typenames) {
            syntaxStr.append("\nnamespace " + typeStr + " {");
        }

        syntaxStr.append("\nnamespace " + std::string(NS_FUNCTION) + " {")
                 .append("\nnamespace " + ast().function + " {")
                 .append("\n    inline constexpr std::string_view id = \"")
                 .append(ast().function)
                 .append("\";");

        for (std::size_t i = 0; i < m_signaturesTy.size(); i++) {
            syntaxStr.append("\n    inline constexpr std::string_view sign" + std::to_string(i) + " = \"")
                     .append(m_signaturesTy[i])
                     .append("\";");
        }
        syntaxStr.append("\n}}");

        for (auto& _ : typenames) {
            syntaxStr.append("}");
        }
        syntaxStr.append("}");
        return syntaxStr;
    }


    std::string ASTCodeMeta::toRecordIdentifierSyntax() const
	{
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(ast().record);
        
        std::string syntaxStr = "\nnamespace " + std::string(NS_TYPE) + " {";
        for (const auto& typeStr : typenames) {
            syntaxStr.append("\nnamespace " + typeStr + " {");
        }
        
        syntaxStr.append("\n    inline constexpr std::string_view id = \"")
                 .append(ast().record)
                 .append("\";\n");
        
        for (auto& _ : typenames) {
            syntaxStr.append("}");
        }
        syntaxStr.append("}");
        return syntaxStr;
    }


    std::string ASTCodeMeta::toRegistrationDeclSyntax() const
    {
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(ast().record);

        std::string syntaxStr = "\nnamespace " + std::string(NS_REGISTRATION) + " {"
                                "\nnamespace " + std::string(NS_TYPE) + " {";

        for (const auto& typeStr : typenames) {
            syntaxStr.append("\nnamespace " + typeStr + " {");
        }

        syntaxStr.append("\n    " + std::string(DECL_INIT_REGIS) + "\n");

        for (auto& _ : typenames) {
            syntaxStr.append("}");
        }
        syntaxStr.append("}}");
        return syntaxStr;
    }
}