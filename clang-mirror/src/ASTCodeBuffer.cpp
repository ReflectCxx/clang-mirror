

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
        auto [itr, _] = pFnMetaMap.try_emplace(pRecordStr, ASTRecordMeta{ index_none, pRecordStr });
        return itr->second;
    }


    ASTFnMeta& ASTCodeBuffer::addFunctionCodeMeta(CxxFunctionsMap& pFnMetaMap, const ASTFnMeta& pFnMeta)
    {
        auto [itr, _] = pFnMetaMap.try_emplace(pFnMeta.ast.function, pFnMeta);
        return itr->second;
    }


    void ASTCodeBuffer::addFunction(MetaKind pMk, const ASTObj& pAst, const std::string& pRecord,
			                        const std::string& pReturn, const std::string& pParams)
    {
        ASTFnMeta* codeMeta = nullptr;
        if (pMk == MetaKind::NonMemberFn) {
            codeMeta = &addFunctionCodeMeta(m_freeFnsMap, ASTFnMeta { 
                .isCtor = false,
                .ast = pAst 
            });
        }
        else if (pMk != MetaKind::None) {
            auto& typeMeta = getRecordCodeMeta(m_recordsMap, pRecord);
            codeMeta = &addFunctionCodeMeta(typeMeta.methods, ASTFnMeta { 
                .isCtor = (pMk == MetaKind::Ctor),
                .ast = pAst
            });
        }

        if (codeMeta) {
            codeMeta->signatures.push_back({
                .metaKind = pMk,
                .returnStr = pReturn,
                .paramsStr = (pParams.empty() ? "void" : pParams)
            });
            m_incFiles.insert(pAst.headers.begin(), pAst.headers.end());
        }
    }
}