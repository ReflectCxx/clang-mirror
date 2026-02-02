
#include <map>
#include <vector>
#include <string>

#include "Constants.h"
#include "StringUtils.h"
#include "ASTCodeMeta.h"

namespace clmr
{
    std::string ASTCodeMeta::toMethodIdentifierSyntax() const
    {
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(ast.record);

        std::string syntaxStr = "\nnamespace " + std::string(NS_TYPE) + " {";
        for (const auto& typeStr : typenames) {
            syntaxStr.append("\nnamespace " + typeStr + " {");
        }

        syntaxStr.append("\nnamespace " + std::string(NS_FUNCTION) + " {")
                 .append("\nnamespace " + ast.function + " {")
                 .append("\n    inline constexpr std::string_view id = \"")
                 .append(ast.function)
                 .append("\";");

        for (std::size_t i = 0; i < signatures.size(); i++) {
            syntaxStr.append("\n    inline constexpr std::string_view sign" + std::to_string(i) + " = \"")
                     .append(signatures[i])
                     .append("\";");
        }
        syntaxStr.append("\n}}");

        for (auto& _ : typenames) {
            syntaxStr.append("}");
        }
        syntaxStr.append("}");
        return syntaxStr;
    }


    std::string ASTCodeMeta::toRegistrationDeclSyntax() const
    {
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(ast.record);

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