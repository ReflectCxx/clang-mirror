
#include "ASTCodeMeta.h"
#include "ASTCodePrinter.h"
#include "StringUtils.h"

namespace clmr
{
    void ASTCodePrint::outFreeFnsDecls(const CxxFunctionsMap& pFnsMap, std::ofstream& pOut)
    {
        for (const auto& itr : pFnsMap) {
            if (!itr.second.signatures.empty()) {
                pOut << "\nnamespace " + std::string(NS_REGISTER) + " {";
                pOut << printFreeFnsInitDecls(itr.second.ast.function);
                pOut << "}\n\n";
            }
        }
    }


    void ASTCodePrint::outFreeFnsInitDefs(const CxxFunctionsMap& pFnsMap, std::ofstream& pOut)
    {
        for (const auto& itr : pFnsMap) {
            if (!itr.second.signatures.empty()) {
                pOut << "\nnamespace " + std::string(NS_REGISTER) + " {";
                pOut << printFreeFnsInitDefs(itr.second);
                pOut << "}\n\n";
            }
        }
    }


    void ASTCodePrint::outRecordInitDefs(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut)
    {
        for (const auto& itr : pRecodsMap) {
            pOut << "\nnamespace " + std::string(NS_REGISTER) + " {";
            pOut << printTypeRecordInitDefs(itr.second);
            pOut << "}\n\n";
        }
    }


    void ASTCodePrint::outRecordInitDecls(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut) 
    {
        if (!pRecodsMap.begin()->second.methods.empty()) {
            for (const auto& itr : pRecodsMap) {

                const auto& methodMap = itr.second.methods;
                const auto& fnMeta = methodMap.begin()->second;

                pOut << "\nnamespace " + std::string(NS_REGISTER) + " {";
                pOut << printTypeRecordInitDecls(fnMeta.ast.record);
                pOut << "}\n\n";
            }
        }
    }


    void ASTCodePrint::outFreeFunctionIDs(const CxxFunctionsMap& pFunctionsMap, std::ofstream& pOut)
    {
        for (auto it = pFunctionsMap.begin(); it != pFunctionsMap.end(); ++it)
        {
            const auto& metaFn = it->second;
            if (metaFn.ast.metaKind == MetaKind::NonMemberFn) {

                std::string codeStr = "\nnamespace " + std::string(NS_FUNCTION) + " {";
                codeStr.append(printNamespaceFnIDs(metaFn))
                       .append("}");

                pOut << codeStr << "\n";
            }
        }
    }


    std::string ASTCodePrint::printRecordNamespaceFnIDs(const ASTCodeMeta& pMeta)
    {
        std::string codeStr;
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(pMeta.ast.record);
        for (const auto& typeStr : typenames) {
            codeStr.append("\nnamespace " + typeStr + " {");
        }

        codeStr.append("\nnamespace " + std::string(NS_FUNCTION) + " {")
               .append("\nnamespace " + pMeta.ast.function + " {")
               .append(printFreeFnsIDs(pMeta))
               .append("\n}}");

        for (auto& _ : typenames) {
            codeStr.append("}");
        }
        return codeStr;
    }


    void ASTCodePrint::outMemberFunctionIDs(const CxxFunctionsMap& pMethodsMap, std::ofstream& pOut)
    {
        for (auto it = pMethodsMap.begin(); it != pMethodsMap.end(); ++it)
        {
            const auto& metaFn = it->second;
            if (metaFn.ast.metaKind != MetaKind::Ctor) {

                std::string codeStr;
                codeStr.append("\nnamespace " + std::string(NS_TYPE) + " {")
                       .append(printRecordNamespaceFnIDs(it->second))
                       .append("}");

                pOut << codeStr << "\n";
            }
        }
    }


    std::string ASTCodePrint::printFreeFnsIDs(const ASTCodeMeta& pMeta)
    {
        std::string codeStr;
        codeStr.append("\n    inline constexpr std::string_view id = \"")
               .append(pMeta.ast.function)
               .append("\";")
               .append("\n    inline constexpr std::string_view signatures = \"{\"\n");

        auto size = pMeta.signatures.size();
        for (std::size_t i = 0; i < size; i++) 
        {
            codeStr.append("        \"sign" + std::to_string(i) + ": ")
                   .append(pMeta.signatures[i].returnType)
                   .append("(" + pMeta.signatures[i].paramsType + ")");

            if (i < size - 1) {
                codeStr.append(",\"\n");
            }
            else {
                codeStr.append("\"");
            }
        }
        codeStr.append("\n    \"}\";");
        return codeStr;
    }


    void ASTCodePrint::outTypeRecordIDs(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut)
    {
        for (const auto& itr : pRecodsMap) {

            const auto& methodMap = itr.second.methods;
            const auto& fnMeta = methodMap.begin()->second;

            std::string codeStr;
            codeStr.append("\nnamespace " + std::string(NS_TYPE) + " {")
                   .append(printTypeRecordIDs(fnMeta))
                   .append("}");

            pOut << codeStr << "\n";
            outMemberFunctionIDs(methodMap, pOut);
            pOut << "\n";
        }
    }

    
    std::string ASTCodePrint::printFreeFnsInitDecls(const std::string& pFnName)
    {
        std::string codeStr;
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(pFnName);
        for (const auto& typeStr : typenames) {
            codeStr.append("\nnamespace " + typeStr + " {");
        }
        codeStr.append("\n    " + std::string(REGIS_INIT_DECL) + ";\n");
        for (auto& _ : typenames) {
            codeStr.append("}");
        }
        return codeStr;
    }


    std::string ASTCodePrint::printFreeFnsInitDefs(const ASTCodeMeta& pMeta)
    {
        std::string codeStr;
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(pMeta.ast.function);
        for (const auto& typeStr : typenames) {
            codeStr.append("\nnamespace " + typeStr + " {");
        }
        codeStr.append("\n    " + std::string(REGIS_INIT_DEFN) + " {");
        //write code here.
        codeStr.append("\n    }\n");
        for (auto& _ : typenames) {
            codeStr.append("}");
        }
        return codeStr;
    }


    std::string ASTCodePrint::printTypeRecordInitDefs(const ASTRecordMeta& pMeta)
    {
        std::string codeStr;
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(pMeta.typeStr);
        for (const auto& typeStr : typenames) {
            codeStr.append("\nnamespace " + typeStr + " {");
        }

        codeStr.append("\n    " + std::string(REGIS_INIT_DEFN) + " {")
               .append("\n\n        fns.push_back(rtl::type().record<").append(pMeta.typeStr)
               .append(">(\"")
               .append(pMeta.typeStr)
               .append("\").build());");

        codeStr.append("\n    }\n");
        for (auto& _ : typenames) {
            codeStr.append("}");
        }
        return codeStr;
    }


    std::string ASTCodePrint::printTypeRecordIDs(const ASTCodeMeta& pMeta)
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


    std::string ASTCodePrint::printTypeRecordInitDecls(const std::string& pRecordName)
    {
        std::string codeStr;
        std::vector<std::string> typenames = StringUtils::splitQualifiedName(pRecordName);
        for (const auto& typeStr : typenames) {
            codeStr.append("\nnamespace " + typeStr + " {");
        }

        codeStr.append("\n    " + std::string(REGIS_INIT_DECL) + ";\n");

        for (auto& _ : typenames) {
            codeStr.append("}");
        }
        return codeStr;
    }


    std::string ASTCodePrint::printNamespaceFnIDs(const ASTCodeMeta& pMeta)
    {
        std::vector<std::string> typnames = StringUtils::splitQualifiedName(pMeta.ast.function);
        std::string fnName = typnames.back();
        typnames.pop_back();

        std::string codeStr;
        for (const auto& typeStr : typnames) {
            codeStr.append("\nnamespace " + typeStr + " {");
        }

        codeStr.append("\nnamespace " + fnName + " {")
               .append(printFreeFnsIDs(pMeta))
               .append("\n}");

        for (auto& _ : typnames) {
            codeStr.append("}");
        }

        return codeStr;
    }
}