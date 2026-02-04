
#include "ASTCodeMeta.h"
#include "ASTCodePrinter.h"
#include "StringUtils.h"


namespace clmr
{
    void ASTCodePrint::closeNS(std::string& pCodeStr, std::size_t pCount)
    {
        for (int i = 0; i < pCount; i++) {
            pCodeStr.append("}");
        }
    }

    std::size_t ASTCodePrint::openNS(std::string& pCodeStr, const std::string& pType)
    {
        std::vector<std::string> typnames = StringUtils::splitQualifiedName(pType);
        for (const auto& typeStr : typnames) {
            pCodeStr.append("\nnamespace " + typeStr + " {");
        }
        return typnames.size();
    }


    std::string clmr::ASTCodePrint::getSignaturesJSON(const std::vector<ASTFnSign>& pSigns)
    {
        auto size = pSigns.size();
        std::string codeStr = "\"{\"\n";

        for (std::size_t i = 0; i < size; i++) 
        {
            codeStr.append("        \"sign" + std::to_string(i) + ": ")
                   .append(pSigns[i].returnType)
                   .append("(" + pSigns[i].paramsType + ")")
                   .append((i < size - 1) ? ",\"\n" : "\"");
        }
        return codeStr.append("\n    \"}\"");
    }
}



namespace clmr
{
    void ASTCodePrint::outRecordInitDefs(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut)
    {
        for (const auto& itr : pRecodsMap) {
            pOut << "\nnamespace " + std::string(NS_REGISTER) + " {";
            pOut << printRecordInitDefs(itr.second);
            pOut << "}\n\n";
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


    void ASTCodePrint::outRecordInitDecls(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut)
    {
        if (!pRecodsMap.begin()->second.methods.empty()) {
            for (const auto& itr : pRecodsMap) {

                const auto& methodMap = itr.second.methods;
                const auto& fnMeta = methodMap.begin()->second;

                pOut << "\nnamespace " + std::string(NS_REGISTER) + " {";
                pOut << printRecordInitDecls(fnMeta.ast.record);
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

                std::string codeStr;
                codeStr.append("\nnamespace ").append(NS_FUNCTION).append(" {")
                       .append(printNamespaceFnIDs(metaFn))
                       .append("}");

                pOut << codeStr << "\n";
            }
        }
    }


    void ASTCodePrint::outMemberFunctionIDs(const CxxFunctionsMap& pMethodsMap, std::ofstream& pOut)
    {
        for (auto it = pMethodsMap.begin(); it != pMethodsMap.end(); ++it)
        {
            const auto& metaFn = it->second;
            if (metaFn.ast.metaKind != MetaKind::Ctor) {

                std::string codeStr;
                codeStr.append("\nnamespace ").append(NS_TYPE).append(" {")
                       .append(printRecordNamespaceFnIDs(it->second))
                       .append("}");

                pOut << codeStr << "\n";
            }
        }
    }


    void ASTCodePrint::outTypeRecordIDs(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut)
    {
        for (const auto& itr : pRecodsMap) {

            const auto& methodMap = itr.second.methods;
            const auto& fnMeta = methodMap.begin()->second;

            std::string codeStr;
            codeStr.append("\nnamespace " + std::string(NS_TYPE) + " {")
                   .append(printRecordIDs(fnMeta))
                   .append("}");

            pOut << codeStr << "\n";
            outMemberFunctionIDs(methodMap, pOut);
            pOut << "\n";
        }
    }
}



namespace clmr
{
    std::string ASTCodePrint::printFreeFnsInitDecls(const std::string& pFnName)
    {
        std::string codeStr;
        int nscount = openNS(codeStr, pFnName);
        codeStr.append("\n    ").append(REGIS_INIT_DECL).append(";\n");

        closeNS(codeStr, nscount);
        return codeStr;
    }


    std::string ASTCodePrint::printRecordInitDecls(const std::string& pRecordName)
    {
        std::string codeStr;
        int nscount = openNS(codeStr, pRecordName);
        codeStr.append("\n    " + std::string(REGIS_INIT_DECL) + ";\n");

        closeNS(codeStr, nscount);
        return codeStr;
    }


    std::string ASTCodePrint::printFreeFnsIDs(const ASTCodeMeta& pMeta)
    {
        return std::string("\n    inline constexpr std::string_view id = \"")
               .append(pMeta.ast.function)
               .append("\";")
               .append("\n    inline constexpr std::string_view signatures = ")
               .append(getSignaturesJSON(pMeta.signatures))
               .append(";");
    }


    std::string ASTCodePrint::printRecordNamespaceFnIDs(const ASTCodeMeta& pMeta)
    {
        std::string codeStr;
        int nscount = openNS(codeStr, pMeta.ast.record);

        codeStr.append("\nnamespace " + std::string(NS_FUNCTION) + " {")
               .append("\nnamespace " + pMeta.ast.function + " {")
               .append(printFreeFnsIDs(pMeta))
               .append("\n}}");

        closeNS(codeStr, nscount);
        return codeStr;
    }


    std::string ASTCodePrint::printRecordIDs(const ASTCodeMeta& pMeta)
    {
        std::string codeStr;
        int nscount = openNS(codeStr, pMeta.ast.record);

        codeStr.append("\n    inline constexpr std::string_view id = \"")
               .append(pMeta.ast.record)
               .append("\";\n");

        closeNS(codeStr, nscount);
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

        closeNS(codeStr, typnames.size());
        return codeStr;
    }



    std::string ASTCodePrint::printFreeFnsInitDefs(const ASTCodeMeta& pMeta)
    {
        std::string codeStr;
        int nscount = openNS(codeStr, pMeta.ast.function);
        
        codeStr.append("\n    ").append(REGIS_INIT_DEFN).append(" {");
        //write code here.
        codeStr.append("\n    }\n");

        closeNS(codeStr, nscount);
        return codeStr;
    }


    std::string ASTCodePrint::printRecordInitDefs(const ASTRecordMeta& pMeta)
    {
        std::string codeStr;
        int nscount = openNS(codeStr, pMeta.record);

        auto idStr = std::string(NS_CXX).append("::").append(NS_TYPE)
                                        .append("::").append(pMeta.record)
                                        .append("::").append(VAR_ID);

        codeStr.append("\n    " + std::string(REGIS_INIT_DEFN) + " {\n\n")
               .append("        fns.push_back(rtl::type().record<" + pMeta.record + ">(" + idStr + ")"
                     "\n                                 .build());");

        for (auto& it : pMeta.methods) {

            auto name = (pMeta.record + "::" + it.first);
            auto fIdStr = std::string(NS_CXX).append("::").append(NS_TYPE)
                                             .append("::").append(pMeta.record)
                                             .append("::").append(NS_FUNCTION)
                                             .append("::").append(VAR_ID);

            if (it.second.signatures.size() == 1) {
                codeStr.append("\n\n        fns.push_back(rtl::type().member<").append(pMeta.record).append(">()"
                                 "\n                                 .method(").append(fIdStr).append(")"
                                 "\n                                 ").append(".build(&").append(name).append("));");
            }
        }
        codeStr.append("\n    }\n");

        closeNS(codeStr, nscount);
        return codeStr;
    }
}