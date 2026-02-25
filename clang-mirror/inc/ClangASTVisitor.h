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

        void addReflectableEntity(clang::FunctionDecl* pFnDecl, const std::string& pHeader);

        bool isHeaderReachableForType(const clang::QualType& pQT,
                                      const clang::FunctionDecl* pFnDecl,
                                      const std::string& pTypeStr,
                                      const std::string& pSrcHeader);
    public:

        ClangASTVisitor(const std::string& pSrcFile, ClangPPCallbacks& pPP);

        bool VisitFunctionDecl(clang::FunctionDecl* pFnDecl);
    };
}