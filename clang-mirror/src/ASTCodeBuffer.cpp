

#include "ASTCodeBuffer.h"

namespace clmirror
{
    ASTCodeBuffer::ASTCodeBuffer(const std::string& pSrcFile)
        : m_errorsFound(false)
        , m_srcFile(pSrcFile)
        , m_recordsMap(RtlRecordsMap())
        , m_freeFnsMap(RtlFunctionsMap())
        , m_incFiles(std::unordered_set<std::string>())
    { }


    ASTCodeMeta& ASTCodeBuffer::addFunctionCodeMeta(RtlFunctionsMap& pFnMetaMap, const ASTCodeMeta& pFnMeta)
    {
        auto itr = pFnMetaMap.find(pFnMeta.m_function);
        if (itr == pFnMetaMap.end()) {
            return pFnMetaMap.emplace(pFnMeta.m_function, pFnMeta).first->second;
        }
        else {
            return itr->second;
        }
    }


    ASTMetaType& ASTCodeBuffer::getRecordCodeMeta(RtlRecordsMap& pFnMetaMap, const std::string& pTypeStr)
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


    void ASTCodeBuffer::addFunction(MetaKind pMetaKind, const std::string& pHeaderFile, const std::string& pRecord,
                                       const std::string& pFnName, const std::string& pParamTypes)
    {
        if (pMetaKind == MetaKind::NonMemberFn)
        {
            auto& codeMeta = addFunctionCodeMeta(m_freeFnsMap, ASTCodeMeta{
                    .m_metaKind = pMetaKind,
                    .m_header = pHeaderFile,
                    .m_record = pRecord,
                    .m_function = pFnName,
                    .m_argTypes = std::vector<std::string>()
            });
            codeMeta.m_argTypes.push_back(pParamTypes.empty() ? "void" : pParamTypes);
        }
        else if (pMetaKind != MetaKind::None)
        {
            auto& recordMeta = getRecordCodeMeta(m_recordsMap, pRecord);
            auto& codeMeta = addFunctionCodeMeta(recordMeta.methods, ASTCodeMeta{
                    .m_metaKind = pMetaKind,
                    .m_header = pHeaderFile,
                    .m_record = pRecord,
                    .m_function = pFnName,
                    .m_argTypes = std::vector<std::string>()
            });
            codeMeta.m_argTypes.push_back(pParamTypes.empty() ? "void" : pParamTypes);
        }
        m_incFiles.insert(pHeaderFile);
    }
}