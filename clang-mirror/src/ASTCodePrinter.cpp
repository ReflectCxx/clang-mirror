
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
        for (auto it = pFunctionsMap.begin(); it != pFunctionsMap.end(); ++it)
        {
            const auto& metaFn = it->second;
            if (metaFn.m_metaKind == MetaKind::NonMemberFn) {
                pOut << metaFn.toFunctionIdentifierSyntax() << "\n";
            }
        }
    }


    void ASTCodePrinter::printRecordTypeIds(const RtlRecordsMap& pRecodsMap, std::fstream& pOut)
    {
        for (const auto& itr : pRecodsMap) {

            const auto& methodMap = itr.second.methods;
            const auto& fnMeta = methodMap.begin()->second;

            pOut << fnMeta.toRecordIdentifierSyntax() << "\n";
            for (auto it = methodMap.begin(); it != methodMap.end(); ++it)
            {
                const auto& metaFn = it->second;
                if (metaFn.m_metaKind != MetaKind::Ctor) {
                    pOut << it->second.toMethodIdentifierSyntax() << "\n";
                }
            }
            pOut << "\n";
        }
    }
}