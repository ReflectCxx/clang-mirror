#pragma once

#include <map>
#include "clang/AST/RecursiveASTVisitor.h"

namespace clmr {
    class ASTCodeBuffer;
    class ClangPPCallbacks;
}

namespace clmr {

    using ErrStr = std::pair<RegErr, std::string>;

    class ClangASTVisitor : public clang::RecursiveASTVisitor<ClangASTVisitor>
    {
        const std::string m_srcFile;
        ClangPPCallbacks& m_preProcessor;

        std::optional<std::string> getHashIncludeStr(const clang::TagDecl* pTypeDecl, std::string_view pTypeStr,
                                                     bool pShouldBePublic);

        RegErr addReflectableEntity(const clang::FunctionDecl* pFnDecl,
                                    const clang::FileEntry* pDeclFile);

        RegErr isHeaderReachableForType(const clang::QualType& pQT,
                                        const clang::FunctionDecl* pFnDecl,
                                        const std::string& pTypeStr,
                                        const clang::FileEntry* pSrcHeader);

        RegErr extractArgsAndItsHeaders(const clang::FunctionDecl *pFnDecl,
                                        const clang::FileEntry* pDeclFile,
                                        std::vector<std::string>& pArgsStrs,
                                        std::vector<std::string>& pHeaders);

        ErrStr getReturnStrAndItsHeaders(const clang::FunctionDecl *pFnDecl,
                                         const clang::FileEntry* pDeclFile,
                                         std::vector<std::string>& pHeaders);
    public:

        ClangASTVisitor(const std::string& pSrcFile, ClangPPCallbacks& pPP);

        bool VisitFunctionDecl(clang::FunctionDecl* pFnDecl);
    };
}