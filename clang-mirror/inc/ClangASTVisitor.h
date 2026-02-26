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

        void addReflectableEntity(const clang::FunctionDecl* pFnDecl,
                                  const clang::FileEntry* pDeclFile);

        bool isHeaderReachableForType(const clang::QualType& pQT,
                                      const clang::FunctionDecl* pFnDecl,
                                      const std::string& pTypeStr,
                                      const clang::FileEntry* pSrcHeader);
    public:

        ClangASTVisitor(const std::string& pSrcFile, ClangPPCallbacks& pPP);

        bool VisitFunctionDecl(clang::FunctionDecl* pFnDecl);
    };
}