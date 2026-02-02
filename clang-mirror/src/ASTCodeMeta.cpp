
#include <map>
#include <vector>
#include <string>

#include "Constants.h"
#include "StringUtils.h"
#include "ASTCodeMeta.h"

namespace clmr
{
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