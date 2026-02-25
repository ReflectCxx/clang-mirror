#pragma once

#include <string>
#include <optional>

#include "ClangPPCallbacks.h"

#include "clang/Lex/Lexer.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/RecursiveASTVisitor.h"


namespace clmr {

    struct ASTDeclsUtils
    {
        static void polishTypeStr(std::string& pTypeStr);
        static bool isInUserCode(clang::NamedDecl* pNameDecl);

        static std::string extractParentTypeName(clang::FunctionDecl* pFuncDecl);
        static std::string extractQualifiedTypeName(const clang::QualType& pQType);

        static const clang::FileEntry* resolveHeaderFromType(const clang::QualType& pQT,
                                                             const clang::ASTContext& pContext,
                                                             const ClangPPCallbacks& pPP);

        static const clang::FileEntry* resolveHeaderFromDecl(const clang::NamedDecl* pDecl,
                                                             const clang::SourceManager& pSrcMgr,
                                                             const ClangPPCallbacks& pPP);

        using optstr = std::optional<std::string>;
        static optstr getTypeDefAliasForType(const clang::QualType& pQType,
                                             std::unordered_map<std::string, std::string>& pTemplateTypeDefs);
    };
}