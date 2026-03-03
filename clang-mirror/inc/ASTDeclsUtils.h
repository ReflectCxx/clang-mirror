#pragma once

#include <string>
#include <optional>

#include "ClangPPCallbacks.h"

#include "clang/Lex/Lexer.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/RecursiveASTVisitor.h"


namespace clmr {

    using errfile = std::pair<RegErr, const clang::FileEntry*>;

    struct ASTDeclsUtils
    {
        static void polishTypeStr(std::string& pTypeStr);

        static bool isInUserCode(clang::NamedDecl* pNameDecl);

        static std::string extractQualifiedTypeName(const clang::QualType& pQType);

        static std::string extractParentTypeName(const clang::FunctionDecl* pFuncDecl);

        static errfile getHeaderDefiningType(const clang::QualType& pQT,
                                             const clang::ASTContext& pContext);

        static std::pair<clmr::MetaKind, std::string> getNameAndMetaKind(const clang::FunctionDecl* pFnDecl);

    private:

        using optstr = std::optional<std::string>;
        static optstr getTypeDefAliasForType(const clang::QualType& pQType,
                                             std::unordered_map<std::string, std::string>& pTemplateTypeDefs);

        static errfile resolveHeaderFromDecl(const clang::NamedDecl* pDecl,
                                             const clang::SourceManager& pSrcMgr);
    };
}