

#include "ASTCodeBuffer.h"

namespace clmr
{
    ASTCodeBuffer::ASTCodeBuffer(const std::string& pSrcFile)
        : m_errorsFound(false)
        , m_srcFile(pSrcFile)
        , m_srcFileIndex(index_none)
        , m_recordsMap(CxxRecordsMap())
        , m_freeFnsMap(CxxFunctionsMap())
        , m_incFiles(std::unordered_set<std::string>())
    { }


    void ASTCodeBuffer::setErrorsFound(bool pErrorsFound) {
        m_errorsFound = pErrorsFound;
    }

    void ASTCodeBuffer::setSrcFileIndex(std::size_t pIndex) {
        m_srcFileIndex = pIndex;
    }

    ASTRecordMeta& ASTCodeBuffer::getRecordCodeMeta(CxxRecordsMap& pFnMetaMap, const std::string& pRecordStr)
    {
        auto [itr, _] = pFnMetaMap.try_emplace(pRecordStr, ASTRecordMeta{ pRecordStr });
        return itr->second;
    }


    ASTCodeMeta& ASTCodeBuffer::addFunctionCodeMeta(CxxFunctionsMap& pFnMetaMap, const ASTCodeMeta& pFnMeta)
    {
        auto [itr, _] = pFnMetaMap.try_emplace(pFnMeta.ast.function, pFnMeta);
        return itr->second;
    }


    void ASTCodeBuffer::addFunction(MetaKind pMK, const ASTObj& pAst, const std::string& pRecord,
			                        const std::string& pReturn, const std::string& pParams)
    {
        ASTCodeMeta* codeMeta = nullptr;
        if (pMK == MetaKind::NonMemberFn) {
            codeMeta = &addFunctionCodeMeta(m_freeFnsMap, ASTCodeMeta{ 
                .isCtor = false,
                .ast = pAst 
            });
        }
        else if (pMK != MetaKind::None) {
            auto& typeMeta = getRecordCodeMeta(m_recordsMap, pRecord);
            codeMeta = &addFunctionCodeMeta(typeMeta.methods, ASTCodeMeta{ 
                .isCtor = (pMK == MetaKind::Ctor),
                .ast = pAst
            });
        }

        if (codeMeta) {
            codeMeta->signatures.push_back({
                .metaKind = pMK,
                .returnStr = pReturn,
                .paramsStr = (pParams.empty() ? "void" : pParams)
            });
            m_incFiles.insert(pAst.header);
        }
    }
}