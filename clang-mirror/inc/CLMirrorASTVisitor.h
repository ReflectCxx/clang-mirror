#pragma once

#include <map>
#include "clang/AST/RecursiveASTVisitor.h"

namespace clmirror {
    class ASTCodeBuffer;
}

namespace clmirror {

    class CLMirrorASTVisitor : public clang::RecursiveASTVisitor<CLMirrorASTVisitor>
    {
        const std::string m_srcFile;
    public:

        CLMirrorASTVisitor(const std::string& pSrcFile);

        bool VisitFunctionDecl(clang::FunctionDecl* pFuncDecl);
    };
}