#pragma once

#include <string>
#include <optional>

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
        static const std::optional<std::string> getTypeDefAliasForType(const clang::QualType& pQType, 
                                                                       std::unordered_map<std::string, std::string>& pTemplateTypeDefs);
    };
}