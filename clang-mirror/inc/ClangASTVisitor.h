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
        const ClangPPCallbacks& m_preProcessor;

        void addReflectableEntity(clang::FunctionDecl* pFuncDecl, const std::string& pHeader);

    public:

        ClangASTVisitor(const std::string& pSrcFile, const ClangPPCallbacks& pPP);

        bool VisitFunctionDecl(clang::FunctionDecl* pFuncDecl);
    };
}