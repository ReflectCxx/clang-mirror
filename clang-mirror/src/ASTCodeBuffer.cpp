

#include "ASTCodeBuffer.h"

namespace clmr
{
    ASTCodeBuffer::ASTCodeBuffer(const std::string& pSrcFile)
        : m_errorsFound(false)
        , m_srcFile(pSrcFile)
        , m_recordsMap(CxxRecordsMap())
        , m_freeFnsMap(CxxFunctionsMap())
        , m_incFiles(std::unordered_set<std::string>())
    { }


    ASTCodeMeta& ASTCodeBuffer::addFunctionCodeMeta(CxxFunctionsMap& pFnMetaMap, const ASTCodeMeta& pFnMeta)
    {
        auto itr = pFnMetaMap.find(pFnMeta.ast.function);
        if (itr == pFnMetaMap.end()) {
            return pFnMetaMap.emplace(pFnMeta.ast.function, pFnMeta).first->second;
        }
        else {
            return itr->second;
        }
    }


    ASTRecordMeta& ASTCodeBuffer::getRecordCodeMeta(CxxRecordsMap& pFnMetaMap, const std::string& pTypeStr)
    {
        const auto& itr = pFnMetaMap.find(pTypeStr);
        if (itr == pFnMetaMap.end())
        {
            return pFnMetaMap.emplace(pTypeStr, ASTRecordMeta{
                    .typeStr = pTypeStr,
                    .methods = ASTRecordMeta::MemberFnsMap()
            }).first->second;
        }
        else {
            return itr->second;
        }
    }


    void ASTCodeBuffer::addFunction(const ASTObj& pAst, const std::string& pReturn, const std::string& pParams)
    {
        if (pAst.metaKind == MetaKind::NonMemberFn)
        {
            auto& codeMeta = addFunctionCodeMeta(m_freeFnsMap, ASTCodeMeta{
                    .ast = pAst,
                    .signatures = std::vector<std::string>()
            });
            codeMeta.signatures.push_back(pParams.empty() ? "void" : pParams);
        }
        else if (pAst.metaKind != MetaKind::None)
        {
            auto& typeMeta = getRecordCodeMeta(m_recordsMap, pAst.record);
            auto& codeMeta = addFunctionCodeMeta(typeMeta.methods, ASTCodeMeta{
                    .ast = pAst,
                    .signatures = std::vector<std::string>()
            });
            codeMeta.signatures.push_back(pParams.empty() ? "void" : pParams);
        }
        m_incFiles.insert(pAst.header);
    }
}