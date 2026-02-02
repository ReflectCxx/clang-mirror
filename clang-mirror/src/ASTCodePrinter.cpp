
#include <unordered_set>

#include "ASTCodeMeta.h"
#include "ASTCodePrinter.h"

namespace clmirror
{
    void ASTCodePrinter::printRegistrationDecls(const RtlRecordsMap& pRecodsMap, std::fstream& pOut) 
    {
        for (const auto& itr : pRecodsMap) {

            const auto& methodMap = itr.second.methods;
            const auto& fnMeta = methodMap.begin()->second;
            pOut << fnMeta.toRegistrationDeclSyntax() << "\n";
        }
    }


    void ASTCodePrinter::printFreeFunctionIds(const RtlFunctionsMap& pFunctionsMap, std::fstream& pOut)
    {
        std::unordered_set<std::string> seen;
        for (auto it = pFunctionsMap.begin(); it != pFunctionsMap.end(); ++it)
        {
            const auto& metaFn = it->second;
            if (metaFn.m_metaKind == MetaKind::NonMemberFn) {

                const std::string& key = it->first;
                if (!seen.insert(key).second) {
                    continue;
                }
                pOut << metaFn.toFunctionIdentifierSyntax() << "\n";
            }
        }
    }


    void ASTCodePrinter::printRecordTypeIds(const RtlRecordsMap& pRecodsMap, std::fstream& pOut)
    {
        for (const auto& itr : pRecodsMap) {

            std::unordered_set<std::string> seen;
            const auto& methodMap = itr.second.methods;
            const auto& fnMeta = methodMap.begin()->second;

            pOut << fnMeta.toRecordIdentifierSyntax() << "\n";
            for (auto it = methodMap.begin(); it != methodMap.end(); ++it)
            {
                const auto& metaFn = it->second;
                if (metaFn.m_metaKind != MetaKind::Ctor) {
                    const std::string& key = it->first;
                    if (!seen.insert(key).second) {
                        continue;
                    }
                    pOut << it->second.toMethodIdentifierSyntax() << "\n";
                }
            }
            pOut << "\n";
        }
    }
}