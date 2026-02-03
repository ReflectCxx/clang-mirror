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

    public:

        CLMirrorASTVisitor(const std::string& pSrcFile);

        bool VisitFunctionDecl(clang::FunctionDecl* pFuncDecl);
    };
}