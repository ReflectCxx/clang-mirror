

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
        auto itr = pFnMetaMap.find(pFnMeta.ast().function);
        if (itr == pFnMetaMap.end()) {
            return pFnMetaMap.emplace(pFnMeta.ast().function, pFnMeta).first->second;
        }
        else {
            return itr->second;
        }
    }


    ASTMetaType& ASTCodeBuffer::getRecordCodeMeta(CxxRecordsMap& pFnMetaMap, const std::string& pTypeStr)
    {
        const auto& itr = pFnMetaMap.find(pTypeStr);
        if (itr == pFnMetaMap.end())
        {
            return pFnMetaMap.emplace(pTypeStr, ASTMetaType{
                    .typeStr = pTypeStr,
                    .methods = ASTMetaType::MemberFnsMap()
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
                    .m_astObj = pAst,
                    .m_signaturesTy = std::vector<std::string>()
            });
            codeMeta.m_signaturesTy.push_back(pParams.empty() ? "void" : pParams);
        }
        else if (pAst.metaKind != MetaKind::None)
        {
            auto& typeMeta = getRecordCodeMeta(m_recordsMap, pAst.record);
            auto& codeMeta = addFunctionCodeMeta(typeMeta.methods, ASTCodeMeta{
                    .m_astObj = pAst,
                    .m_signaturesTy = std::vector<std::string>()
            });
            codeMeta.m_signaturesTy.push_back(pParams.empty() ? "void" : pParams);
        }
        m_incFiles.insert(pAst.header);
    }
}