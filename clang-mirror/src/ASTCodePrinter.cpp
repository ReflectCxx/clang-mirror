
#include <cassert>

#include "ASTCodeMeta.h"
#include "ASTCodeBuffer.h"
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
    void ASTCodePrint::outFreeFnsDecls(std::ofstream& pOut, std::size_t pSrcIndex)
    {
        auto ns = std::string(NS_REGS) + std::to_string(pSrcIndex);
        ns .append("::").append(NS_FN);

        pOut << "\nnamespace " + ns + " {"
             << "\n    " << REGIS_INIT_DECL << ";\n"
             << "}\n\n";
    }


    void ASTCodePrint::outRecordInitDecls(std::ofstream& pOut, std::size_t pSrcIndex, std::size_t pTypeIndex)
    {
        auto ns = std::string(NS_REGS) + std::to_string(pSrcIndex);
        ns.append("::").append(NS_TYPE).append(std::to_string(pTypeIndex));

        pOut << "\nnamespace " + ns + " {"
             << "\n    " << REGIS_INIT_DECL << ";\n"
             << "}\n\n";
    }

    void ASTCodePrint::outTypeRecordInitsDefs(ASTCodeBuffer& pCb, std::ofstream& pOut)
    {
        auto index = 0;
        auto srcIndex = pCb.getSrcFileIndex();
        for (auto& itr : pCb.getRecordsMapRef()) {

            auto& recMeta = itr.second;
            recMeta.typeIndex = index++;

            auto ns = std::string(NS_REGS) + std::to_string(srcIndex);
            ns .append("::").append(NS_TYPE).append(std::to_string(recMeta.typeIndex));

            pOut << "\nnamespace " + ns + " {"
                 << recordTypeInitDefs(recMeta)
                 << "}\n\n";
        }
    }


    void ASTCodePrint::outFunctionInitsDefs(const ASTCodeBuffer& pCb, std::ofstream& pOut)
    {
        const auto& srcIndex = pCb.getSrcFileIndex();
        auto ns = std::string(NS_REGS) + std::to_string(srcIndex);
        ns.append("::").append(NS_FN);

        auto& freeFnsMap = pCb.getFreeFunctionsMap();
        if (!freeFnsMap.empty())
        {
            pOut << "\nnamespace " + ns + " {"
                    "\n    " << REGIS_INIT_DEFN << " {\n";

            for (const auto& itr : pCb.getFreeFunctionsMap()) {
                if (!itr.second.signatures.empty()) {
                    pOut << freeFunctionInitDefs(itr.second);
                }
            }
            pOut << "    }\n"
                    "}\n\n";
        }
    }


    void ASTCodePrint::outRegisteredFunctionIDs(const CxxFunctionsMap& pFunctionsMap, std::ofstream& pOut)
    {
        for (auto it = pFunctionsMap.begin(); it != pFunctionsMap.end(); ++it)
        {
            const auto& metaFn = it->second;
            if (metaFn.signatures.front().metaKind == MetaKind::NonMemberFn) {

                auto codeStr = std::string("\nnamespace ");
                codeStr.append(NS_FN).append(" {")
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

                auto codeStr = std::string("\nnamespace ");
                codeStr.append(NS_TYPE).append(" {")
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

            auto codeStr = std::string("\nnamespace ");
            codeStr.append(NS_TYPE)
                   .append(" {")
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

        codeStr.append("\nnamespace " + std::string(NS_FN) + " {")
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
        std::string idStr;
        idStr.append(NS_CXX)
             .append("::").append(NS_FN)
             .append("::").append(pMeta.ast.function)
             .append("::").append(VAR_ID);

        std::string codeStr;
        auto signCount = pMeta.signatures.size();
        if (signCount > 1)
        {
            for (const auto& sign : pMeta.signatures) {
                codeStr.append("\n        fns.push_back(rtl::type().function<").append(sign.paramsStr).append(">(" + idStr + ")"
                               "\n                                 .build(&").append(pMeta.ast.function).append("));\n");
            }
        }
        else
        {
            codeStr.append("\n        fns.push_back(rtl::type().function(" + idStr + ")"
                           "\n                                 .build(&").append(pMeta.ast.function).append("));\n");
        }
        return codeStr;
    }


    std::string ASTCodePrint::recordTypeInitDefs(const ASTRecordMeta& pMeta)
    {
        std::string idStr;
        idStr.append(NS_CXX)
             .append("::").append(NS_TYPE)
             .append("::").append(pMeta.recordStr)
             .append("::").append(VAR_ID);

        std::string codeStr;
        codeStr.append("\n    " + std::string(REGIS_INIT_DEFN) + " {\n\n")
               .append(  "        fns.push_back(rtl::type().record<" + pMeta.recordStr + ">(" + idStr + ")"
                       "\n                                 .build());");

        for (auto& it : pMeta.methods) {

            const ASTCodeMeta& codeMeta = it.second;
            const auto& fIdStr = std::string(NS_CXX).append("::").append(NS_TYPE)
                                                    .append("::").append(pMeta.recordStr)
                                                    .append("::").append(NS_FN)
                                                    .append("::").append(codeMeta.ast.function)
                                                    .append("::").append(VAR_ID);

            codeStr.append(getMethodRegistrationExpr(pMeta.recordStr, fIdStr, codeMeta));
        }
        codeStr.append("\n    }\n");
        return codeStr;
    }
}