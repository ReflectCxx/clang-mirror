
#include <cassert>

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
                   .append(pSigns[i].returnStr)
                   .append("(" + pSigns[i].paramsStr + ")")
                   .append((i < size - 1) ? ",\"\n" : "\"");
        }
        return codeStr.append("\n    \"}\"");
    }


    std::string ASTCodePrint::getMethodRegistrationExpr(const std::string& pTypeID, const std::string& pFnID, const ASTCodeMeta& pMeta)
    {
        assert(!pMeta.signatures.empty());
        auto suffix = [](const ASTFnSign& sign, bool useTemplates)->std::string
        {
            std::string str;
            switch (sign.metaKind) {
                case MetaKind::MemberFnConst: { str = "Const"; break; }
                case MetaKind::MemberFnStatic: { str = "Static"; break; }
                case MetaKind::Ctor: break;
                case MetaKind::MemberFnNonConst: break;
                default: { assert(false); return str; }
            }
            if (useTemplates) {
                str += "<" + sign.paramsStr + ">";
            }
            return str;
        };

        std::string codeStr;
        if (pMeta.isCtor) {
            for (const auto& sign : pMeta.signatures) {
                if (sign.metaKind == MetaKind::Ctor) {
                    codeStr.append("\n\n        fns.push_back(rtl::type().member<").append(pTypeID).append(">()"
                                     "\n                                 .constructor").append(suffix(sign, true)).append("()"
                                                                        ".build());");
                }
            }
        }
        else {
            auto fnStr = (pTypeID + "::" + pMeta.ast.function);
            const bool useTemplate = (pMeta.signatures.size() > 1);
            for (const auto& sign : pMeta.signatures) {
                codeStr.append("\n\n        fns.push_back(rtl::type().member<").append(pTypeID).append(">()"
                                 "\n                                 .method").append(suffix(sign, useTemplate)).append("(").append(pFnID).append(")"
                                 "\n                                 ").append(".build(&").append(fnStr).append("));");
            }
        }
        return codeStr;
    }
}



namespace clmr
{
    void ASTCodePrint::outTypeRecordInitDefs(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut)
    {
        for (const auto& itr : pRecodsMap) {
            pOut << "\nnamespace " + std::string(NS_REGS) + " {";
            pOut << recordTypeInitDefs(itr.second);
            pOut << "}\n\n";
        }
    }


    void ASTCodePrint::outFunctionInitsDefs(const CxxFunctionsMap& pFnsMap, std::ofstream& pOut)
    {
        for (const auto& itr : pFnsMap) {
            if (!itr.second.signatures.empty()) {
                pOut << "\nnamespace " + std::string(NS_REGS) + " {";
                pOut << freeFunctionInitDefs(itr.second);
                pOut << "}\n\n";
            }
        }
    }


    void ASTCodePrint::outFreeFnsDecls(const CxxFunctionsMap& pFnsMap, std::ofstream& pOut)
    {
        for (const auto& itr : pFnsMap) {
            if (!itr.second.signatures.empty()) {
                pOut << "\nnamespace " + std::string(NS_REGS) + " {";
                pOut << freeFunctionInitDecls(itr.second.ast.function);
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

                pOut << "\nnamespace " + std::string(NS_REGS) + " {";
                pOut << recordTypeInitDecls(itr.first);
                pOut << "}\n\n";
            }
        }
    }


    void ASTCodePrint::outRegisteredFunctionIDs(const CxxFunctionsMap& pFunctionsMap, std::ofstream& pOut)
    {
        for (auto it = pFunctionsMap.begin(); it != pFunctionsMap.end(); ++it)
        {
            const auto& metaFn = it->second;
            if (metaFn.signatures.front().metaKind == MetaKind::NonMemberFn) {

                const auto& codeStr = std::string("\nnamespace ")
                                      .append(NS_FUNCTION).append(" {")
                                      .append(getFnIDsWithNameSpaces(metaFn))
                                      .append("}");

                pOut << codeStr << "\n";
            }
        }
    }


    void ASTCodePrint::outMethodIDsWithNamespaces(const std::string& pTypeID, const CxxFunctionsMap& pMethodsMap, std::ofstream& pOut)
    {
        for (auto it = pMethodsMap.begin(); it != pMethodsMap.end(); ++it)
        {
            const auto& metaFn = it->second;
            if (!metaFn.isCtor) {

                const auto& codeStr = std::string("\nnamespace ")
                                      .append(NS_TYPE).append(" {")
                                      .append(getMethodIDDecleration(pTypeID, it->second))
                                      .append("}");
                pOut << codeStr << "\n";
            }
        }
    }


    void ASTCodePrint::outRegisteredTypeRecordIDs(const CxxRecordsMap& pRecodsMap, std::ofstream& pOut)
    {
        for (const auto& itr : pRecodsMap) {

            const auto& methodMap = itr.second.methods;
            const auto& fnMeta = methodMap.begin()->second;

            const auto& codeStr = std::string("\nnamespace ")
                                  .append(NS_TYPE).append(" {")
                                  .append(getTypeIDWithNamespaces(itr.first, fnMeta))
                                  .append("}");
            pOut << codeStr << "\n";
            outMethodIDsWithNamespaces(itr.first, methodMap, pOut);
            pOut << "\n";
        }
    }
}



namespace clmr
{
    std::string ASTCodePrint::freeFunctionInitDecls(const std::string& pFnName)
    {
        std::string codeStr;
        int nscount = openNS(codeStr, pFnName);
        codeStr.append("\n    ").append(REGIS_INIT_DECL).append(";\n");

        closeNS(codeStr, nscount);
        return codeStr;
    }


    std::string ASTCodePrint::recordTypeInitDecls(const std::string& pRecordName)
    {
        std::string codeStr;
        int nscount = openNS(codeStr, pRecordName);
        codeStr.append("\n    " + std::string(REGIS_INIT_DECL) + ";\n");

        closeNS(codeStr, nscount);
        return codeStr;
    }


    std::string ASTCodePrint::getFnIDDeclaration(const ASTCodeMeta& pMeta)
    {
        return std::string("\n    inline constexpr std::string_view id = \"")
               .append(pMeta.ast.function)
               .append("\";")
               .append("\n    inline constexpr std::string_view signatures = ")
               .append(getSignaturesJSON(pMeta.signatures))
               .append(";");
    }


    std::string ASTCodePrint::getMethodIDDecleration(const std::string& pTypeID, const ASTCodeMeta& pMeta)
    {
        std::string codeStr;
        int nscount = openNS(codeStr, pTypeID);

        codeStr.append("\nnamespace " + std::string(NS_FUNCTION) + " {")
               .append("\nnamespace " + pMeta.ast.function + " {")
               .append(getFnIDDeclaration(pMeta))
               .append("\n}}");

        closeNS(codeStr, nscount);
        return codeStr;
    }


    std::string ASTCodePrint::getTypeIDWithNamespaces(const std::string& pTypeID, const ASTCodeMeta& pMeta)
    {
        std::string codeStr;
        int nscount = openNS(codeStr, pTypeID);

        codeStr.append("\n    inline constexpr std::string_view id = \"")
               .append(pTypeID)
               .append("\";\n");

        closeNS(codeStr, nscount);
        return codeStr;
    }


    std::string ASTCodePrint::getFnIDsWithNameSpaces(const ASTCodeMeta& pMeta)
    {
        std::vector<std::string> typnames = StringUtils::splitQualifiedName(pMeta.ast.function);
        std::string fnName = typnames.back();
        typnames.pop_back();

        std::string codeStr;
        for (const auto& typeStr : typnames) {
            codeStr.append("\nnamespace " + typeStr + " {");
        }

        codeStr.append("\nnamespace " + fnName + " {")
               .append(getFnIDDeclaration(pMeta))
               .append("\n}");

        closeNS(codeStr, typnames.size());
        return codeStr;
    }



    std::string ASTCodePrint::freeFunctionInitDefs(const ASTCodeMeta& pMeta)
    {
        std::string codeStr;
        int nscount = openNS(codeStr, pMeta.ast.function);
        
        codeStr.append("\n    ").append(REGIS_INIT_DEFN).append(" {");
        //write code here.
        codeStr.append("\n    }\n");

        closeNS(codeStr, nscount);
        return codeStr;
    }


    std::string ASTCodePrint::recordTypeInitDefs(const ASTRecordMeta& pMeta)
    {
        std::string codeStr;
        int nscount = openNS(codeStr, pMeta.recordStr);

        const auto& idStr = std::string(NS_CXX).append("::").append(NS_TYPE)
                                               .append("::").append(pMeta.recordStr)
                                               .append("::").append(VAR_ID);

        codeStr.append("\n    " + std::string(REGIS_INIT_DEFN) + " {\n\n")
               .append("        fns.push_back(rtl::type().record<" + pMeta.recordStr + ">(" + idStr + ")"
                     "\n                                 .build());");

        for (auto& it : pMeta.methods) {

            const ASTCodeMeta& codeMeta = it.second;
            const auto& fIdStr = std::string(NS_CXX).append("::").append(NS_TYPE)
                                                    .append("::").append(pMeta.recordStr)
                                                    .append("::").append(NS_FUNCTION)
                                                    .append("::").append(codeMeta.ast.function)
                                                    .append("::").append(VAR_ID);

            codeStr.append(getMethodRegistrationExpr(pMeta.recordStr, fIdStr, codeMeta));
        }
        codeStr.append("\n    }\n");

        closeNS(codeStr, nscount);
        return codeStr;
    }
}