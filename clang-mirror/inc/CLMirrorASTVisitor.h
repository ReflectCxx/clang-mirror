#pragma once

#include <map>
#include "clang/AST/RecursiveASTVisitor.h"

namespace clmr {
    class ASTCodeBuffer;
    class CLPPCallbacks;
}

namespace clmr {

    class CLMirrorASTVisitor : public clang::RecursiveASTVisitor<CLMirrorASTVisitor>
    {
        const std::string m_srcFile;
        const CLPPCallbacks& m_preProcessor;

        void addReflectableEntity(clang::FunctionDecl* pFuncDecl, const std::string& pHeader);

    public:

        CLMirrorASTVisitor(const std::string& pSrcFile, const CLPPCallbacks& pPP);

        bool VisitFunctionDecl(clang::FunctionDecl* pFuncDecl);
    };
}