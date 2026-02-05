#pragma once

#include <map>
#include "clang/AST/RecursiveASTVisitor.h"

namespace clmr {
    class ASTCodeBuffer;
}

namespace clmr {

    class CLMirrorASTVisitor : public clang::RecursiveASTVisitor<CLMirrorASTVisitor>
    {
        const std::string m_srcFile;

        void addReflectableEntity(clang::FunctionDecl* pFuncDecl, const std::string& pHeader);

    public:

        CLMirrorASTVisitor(const std::string& pSrcFile);

        bool VisitFunctionDecl(clang::FunctionDecl* pFuncDecl);
    };
}