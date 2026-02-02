
#include "ASTCodeMeta.h"
#include "ASTCodePrinter.h"
#include "StringUtils.h"

namespace clmr
{
    void ASTCodePrinter::printRegistrationDecls(const CxxRecordsMap& pRecodsMap, std::fstream& pOut) 
    {
        for (const auto& itr : pRecodsMap) {

            const auto& methodMap = itr.second.methods;
            const auto& fnMeta = methodMap.begin()->second;
            pOut << fnMeta.toRegistrationDeclSyntax() << "\n";
        }
    }


    void ASTCodePrinter::printFreeFunctions(const CxxFunctionsMap& pFunctionsMap, std::fstream& pOut)
    {
        for (auto it = pFunctionsMap.begin(); it != pFunctionsMap.end(); ++it)
        {
            const auto& metaFn = it->second;
            if (metaFn.ast.metaKind == MetaKind::NonMemberFn) {

                std::string codeStr = "\nnamespace " + std::string(NS_FUNCTION) + " {";
                codeStr.append(printFnNamespace(metaFn))
                       .append("}");

                pOut << codeStr << "\n";
            }
        }
    }


    std::string ASTCodePrinter::printMethodNamespace(const ASTCodeMeta& pMeta)
    {
        std::string codeStr;
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(pMeta.ast.record);
        for (const auto& typeStr : typenames) {
            codeStr.append("\nnamespace " + typeStr + " {");
        }

        codeStr.append("\nnamespace " + std::string(NS_FUNCTION) + " {")
               .append("\nnamespace " + pMeta.ast.function + " {")
               .append(printFnIdDeclarations(pMeta))
               .append("\n}}");

        for (auto& _ : typenames) {
            codeStr.append("}");
        }
        return codeStr;
    }


    void ASTCodePrinter::printMemberFunctions(const CxxFunctionsMap& pMethodsMap, std::fstream& pOut)
    {
        for (auto it = pMethodsMap.begin(); it != pMethodsMap.end(); ++it)
        {
            const auto& metaFn = it->second;
            if (metaFn.ast.metaKind != MetaKind::Ctor) {

                std::string codeStr;
                codeStr.append("\nnamespace " + std::string(NS_TYPE) + " {")
                       .append(printMethodNamespace(it->second))
                       .append("}");

                pOut << codeStr << "\n";
            }
        }
    }


    std::string ASTCodePrinter::printFnIdDeclarations(const ASTCodeMeta& pMeta)
    {
        std::string codeStr;
        codeStr.append("\n    inline constexpr std::string_view id = \"")
               .append(pMeta.ast.function)
               .append("\";");

        for (std::size_t i = 0; i < pMeta.signatures.size(); i++) {
            codeStr.append("\n    inline constexpr std::string_view sign" + std::to_string(i) + " = \"")
                   .append(pMeta.signatures[i])
                   .append("\";");
        }
        return codeStr;
    }


    void ASTCodePrinter::printTypeRecords(const CxxRecordsMap& pRecodsMap, std::fstream& pOut)
    {
        for (const auto& itr : pRecodsMap) {

            const auto& methodMap = itr.second.methods;
            const auto& fnMeta = methodMap.begin()->second;

            std::string codeStr;
            codeStr.append("\nnamespace " + std::string(NS_TYPE) + " {")
                   .append(printTypeIdDeclaration(fnMeta))
                   .append("}");

            pOut << codeStr << "\n";
            printMemberFunctions(methodMap, pOut);
            pOut << "\n";
        }
    }


    std::string ASTCodePrinter::printTypeIdDeclaration(const ASTCodeMeta& pMeta)
    {
        std::string codeStr;
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(pMeta.ast.record);
        for (const auto& typeStr : typenames) {
            codeStr.append("\nnamespace " + typeStr + " {");
        }

        codeStr.append("\n    inline constexpr std::string_view id = \"")
               .append(pMeta.ast.record)
               .append("\";\n");

        for (auto& _ : typenames) {
            codeStr.append("}");
        }
        return codeStr;
    }


    std::string ASTCodePrinter::printFnNamespace(const ASTCodeMeta& pMeta)
    {
        std::vector<std::string> typnames = StringUtils::splitQualifiedName(pMeta.ast.function);
        std::string fnName = typnames.back();
        typnames.pop_back();

        std::string codeStr;
        for (const auto& typeStr : typnames) {
            codeStr.append("\nnamespace " + typeStr + " {");
        }

        codeStr.append("\nnamespace " + fnName + " {")
               .append(printFnIdDeclarations(pMeta))
               .append("\n}");

        for (auto& _ : typnames) {
            codeStr.append("}");
        }

        return codeStr;
    }
}