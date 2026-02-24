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
        static bool isInUserCode(clang::NamedDecl* pNameDecl);
        static bool isDeclFrmCurrentSource(const std::string& pCurSrcFile, clang::Decl* pDecl);

        static void polishTypeStr(std::string& pTypeStr);

        static std::string extractParentTypeName(clang::FunctionDecl* pFuncDecl);
        static std::string extractQualifiedTypeName(const clang::QualType& pQType);

        using optstr = std::optional<std::string>;

        static optstr getHeaderFileForType(clang::VarDecl* pDecl, const ClangPPCallbacks& pPPCb);

        static optstr getHeaderFileForType(clang::FunctionDecl* pDecl, const ClangPPCallbacks& pPPCb);

        static optstr getTypeDefAliasForType(const clang::QualType& pQType,
                                             std::unordered_map<std::string, std::string>& pTemplateTypeDefs);
    };
}