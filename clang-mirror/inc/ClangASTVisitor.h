#pragma once

#include <map>
#include "clang/AST/RecursiveASTVisitor.h"

namespace clmr {
    class ASTCodeBuffer;
    class ClangPPCallbacks;
}

namespace clmr {

    class ClangASTVisitor : public clang::RecursiveASTVisitor<ClangASTVisitor>
    {
        const std::string m_srcFile;
        ClangPPCallbacks& m_preProcessor;

        RegErr addReflectableEntity(const clang::FunctionDecl* pFnDecl,
                                    const clang::FileEntry* pDeclFile);

        RegErr extractArgsAndItsHeaders(const clang::FunctionDecl *pFnDecl,
                                        std::vector<std::string>& pArgsStrs,
                                        std::vector<std::string>& pHeaders);

        RegErr addTypeDefiningHeader(std::vector<std::string>& pHeaders,
                                     const clang::QualType& pQT,
                                     const clang::ASTContext& pCtx);
    public:

        ClangASTVisitor(const std::string& pSrcFile, ClangPPCallbacks& pPP);

        bool VisitFunctionDecl(clang::FunctionDecl* pFnDecl);
    };
}