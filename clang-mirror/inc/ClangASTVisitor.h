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

        std::optional<std::string> getHashIncludeStr(clang::Decl* pTypeDecl, std::string_view pTypeStr,
                                                     bool pShouldBePublic);

        void addReflectableEntity(const clang::FunctionDecl* pFnDecl,
                                  const clang::FileEntry* pDeclFile);

        bool isHeaderReachableForType(const clang::QualType& pQT,
                                      const clang::FunctionDecl* pFnDecl,
                                      const std::string& pTypeStr,
                                      const clang::FileEntry* pSrcHeader);

        bool extractArgsAndItsHeaders(const clang::FunctionDecl *pFnDecl,
                                      const clang::FileEntry* pDeclFile,
                                      std::vector<std::string>& pArgsStrs,
                                      std::vector<std::string>& pHeaders);

        using optstr = std::optional<std::string>;
        optstr getReturnStrAndItsHeaders(const clang::FunctionDecl *pFnDecl,
                                         const clang::FileEntry* pDeclFile,
                                         std::vector<std::string>& pHeaders);
    public:

        ClangASTVisitor(const std::string& pSrcFile, ClangPPCallbacks& pPP);

        bool VisitFunctionDecl(clang::FunctionDecl* pFnDecl);
    };
}