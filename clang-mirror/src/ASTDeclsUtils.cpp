
#include "Constants.h"
#include "StringUtils.h"
#include "ASTDeclsUtils.h"
#include "clang/AST/RecursiveASTVisitor.h"

using namespace clang;

namespace clmr 
{
    bool ASTDeclsUtils::isInUserCode(NamedDecl* pDecl)
    {
        if (!pDecl) {
            return false;
        }

        const SourceManager& SM = pDecl->getASTContext().getSourceManager();
        SourceLocation loc = pDecl->getLocation();
        if (!loc.isValid()) {
            return false;
        }
		
        loc = SM.getSpellingLoc(loc);
        if (SM.isInSystemHeader(loc)) {
            return false;
        }
        return true;
    }


	bool ASTDeclsUtils::isDeclFrmCurrentSource(const std::string& pCurSrcFile, clang::Decl* pDecl)
    {
        std::string currentSrcFile = pCurSrcFile;
        std::transform(currentSrcFile.begin(), currentSrcFile.end(), currentSrcFile.begin(),
            [](unsigned char c)->char {
                return (c == '\\') ? '/' : std::tolower(c);
            });

        const auto& srcManager = pDecl->getASTContext().getSourceManager();
        auto fileLoc = srcManager.getFileLoc(pDecl->getBeginLoc());
        auto declSrcFile = srcManager.getFilename(fileLoc).str();
        std::transform(declSrcFile.begin(), declSrcFile.end(), declSrcFile.begin(),
            [](unsigned char c)->char {
                return (c == '\\') ? '/' : std::tolower(c);
            });
        return (currentSrcFile == declSrcFile);
    }


    std::string ASTDeclsUtils::extractParentTypeName(clang::FunctionDecl* pFnDecl)
    {
        const auto* method = llvm::dyn_cast<clang::CXXMethodDecl>(pFnDecl);
        if (!method)
            return {};

        const clang::CXXRecordDecl* record = method->getParent();
        clang::QualType qt = record->getTypeForDecl()->getCanonicalTypeInternal();
        clang::PrintingPolicy policy(pFnDecl->getASTContext().getLangOpts());
        
        policy.SuppressScope = false;
        policy.SuppressTagKeyword = true;
        policy.FullyQualifiedName = true;
		
        std::string result;
        llvm::raw_string_ostream os(result);
        qt.print(os, policy);
        return os.str();
    }
	
    std::string ASTDeclsUtils::extractParameterType(clang::ParmVarDecl* pParmVarDecl)
    {
        std::unordered_map<std::string, std::string> templateArgsTypeDefs;
        auto typedefStrValue = getTypeDefAliasForType(pParmVarDecl->getOriginalType(), templateArgsTypeDefs);
        if (typedefStrValue.has_value())
        {
            std::string typedefOrgTypeKey;
            const auto& qt = pParmVarDecl->getOriginalType().getCanonicalType().getNonReferenceType();
            if (qt->isFunctionPointerType()) {
                typedefOrgTypeKey = qt.getAsString();
                if (qt.getQualifiers().hasConst()) {
                    typedefStrValue.emplace("const " + (typedefStrValue.value()));
                }
            }
            else if (qt->isPointerType()) {
                typedefOrgTypeKey = qt->getPointeeType().getUnqualifiedType().getAsString();
                StringUtils::removeSubStrings(typedefOrgTypeKey, { CONST, ENUM, CLASS, STRUCT });
            }
            else {
                typedefOrgTypeKey = qt.getUnqualifiedType().getAsString();
                StringUtils::removeSubStrings(typedefOrgTypeKey, { CONST, ENUM, CLASS, STRUCT });
            }
            templateArgsTypeDefs.insert(make_pair(typedefOrgTypeKey, typedefStrValue.value()));
        }
        auto typeStr = pParmVarDecl->getOriginalType().getCanonicalType().getAsString();
        StringUtils::removeSubStrings(typeStr, { ENUM, CLASS, STRUCT });
        for (auto itr : templateArgsTypeDefs)
        {
            const auto& tmpTypeStr = itr.first;
            const auto& tmpTypeDefStr = itr.second;
            StringUtils::replaceSubString(typeStr, tmpTypeStr, tmpTypeDefStr);
        }
        return typeStr;
    }



    const std::optional<std::string> ASTDeclsUtils::getTypeDefAliasForType(const QualType& pQType,
                                                                           std::unordered_map<std::string, std::string>& pTemplateTypeDefs)
    {
        const Type* type = pQType.getTypePtrOrNull();
        if (!type) {
            return std::nullopt;
        }

        switch (type->getTypeClass())
        {
        case Type::TypeClass::Typedef: {

            const auto* tdType = dyn_cast<TypedefType>(type);
            if (!tdType) {
                return std::nullopt;
            }
            const TypedefNameDecl* decl = tdType->getDecl();
            return decl->getQualifiedNameAsString();
        }
        case Type::TypeClass::Elaborated: {

            const ElaboratedType* nxtType = dyn_cast<ElaboratedType>(type);
            return getTypeDefAliasForType(nxtType->getNamedType(), pTemplateTypeDefs);
        }
        case Type::TypeClass::LValueReference: {

            const LValueReferenceType* nxtType = dyn_cast<LValueReferenceType>(type);
            return getTypeDefAliasForType(nxtType->getPointeeType(), pTemplateTypeDefs);
        }
        case Type::TypeClass::Pointer: {

            const PointerType* nxtType = dyn_cast<PointerType>(type);
            return getTypeDefAliasForType(nxtType->getPointeeType(), pTemplateTypeDefs);
        }
        case Type::TypeClass::TemplateSpecialization: {

            const TemplateSpecializationType* templateSpclType = dyn_cast<TemplateSpecializationType>(type);
            for (const auto& templateArg : templateSpclType->template_arguments())
            {
                if (templateArg.getKind() == TemplateArgument::ArgKind::Type)
                {
                    std::unordered_map<std::string, std::string> tempTypeDefs;
                    auto typeDefStr = getTypeDefAliasForType(templateArg.getAsType(), tempTypeDefs);
                    if (typeDefStr.has_value())
                    {
                        QualType qt = templateArg.getAsType()
                                                 .getUnqualifiedType()
                                                 .getNonReferenceType()
                                                 .getCanonicalType();
                        std::string typeStr;
                        if (const auto* RT = qt->getAs<RecordType>()) {
                            typeStr = RT->getDecl()->getQualifiedNameAsString();
                        }
                        else if (const auto* ET = qt->getAs<EnumType>()) {
                            typeStr = ET->getDecl()->getQualifiedNameAsString();
                        }
                        else if (const auto* TT = qt->getAs<TypedefType>()) {
                            typeStr = TT->getDecl()->getQualifiedNameAsString();
                        }
                        else {
                            typeStr = qt.getAsString();
                        }

                        for (auto& itr : pTemplateTypeDefs) {
                            const auto& tmpTypeStr = itr.first;
                            const auto& tmpTypeDefStr = itr.second;
                            StringUtils::replaceSubString(typeStr, tmpTypeStr, tmpTypeDefStr);
                        }
                        pTemplateTypeDefs.insert(std::make_pair(typeStr, typeDefStr.value()));
                    }
                }
            }
            return std::nullopt;
        }
        default: return std::nullopt;
        }
    }




    //const std::optional<std::string> ASTDeclsUtils::getTypeDefAliasForType(const QualType& pQType, std::unordered_map<std::string, std::string>& pTemplateTypeDefs)
    //{
    //    const Type* type = pQType.getTypePtrOrNull();
    //    if (!type) {
    //        return std::nullopt;
    //    }

    //    switch (pQType->getTypeClass())
    //    {
    //    case Type::TypeClass::Typedef: {
    //        return pQType.getAsString();
    //    }
    //    case Type::TypeClass::Elaborated: {
    //        const ElaboratedType* nxtType = dyn_cast<ElaboratedType>(type);
    //        return getTypeDefAliasForType(nxtType->getNamedType(), pTemplateTypeDefs);
    //    }
    //    case Type::TypeClass::LValueReference: {
    //        const LValueReferenceType* nxtType = dyn_cast<LValueReferenceType>(type);
    //        return getTypeDefAliasForType(nxtType->getPointeeType(), pTemplateTypeDefs);
    //    }
    //    case Type::TypeClass::Pointer: {
    //        const PointerType* nxtType = dyn_cast<PointerType>(type);
    //        return getTypeDefAliasForType(nxtType->getPointeeType(), pTemplateTypeDefs);
    //    }
    //    case Type::TypeClass::TemplateSpecialization: {
    //        const TemplateSpecializationType* templateSpclType = dyn_cast<TemplateSpecializationType>(type);
    //        for (const auto& templateArg : templateSpclType->template_arguments())
    //        {
    //            if (templateArg.getKind() == TemplateArgument::ArgKind::Type)
    //            {
    //                std::unordered_map<std::string, std::string> tempTypeDefs;
    //                auto typeDefStr = getTypeDefAliasForType(templateArg.getAsType(), tempTypeDefs);
    //                if (typeDefStr.has_value())
    //                {
    //                    const auto& qt = templateArg.getAsType().getUnqualifiedType().getNonReferenceType().getCanonicalType();
    //                    std::string typeStr = (qt->isPointerType() ? qt->getPointeeType().getUnqualifiedType() : qt).getAsString();
    //                    StringUtils::removeSubStrings(typeStr, { ENUM, CLASS, STRUCT });
    //                    for (auto& itr : pTemplateTypeDefs) {
    //                        const auto& tmpTypeStr = itr.first;
    //                        const auto& tmpTypeDefStr = itr.second;
    //                        StringUtils::replaceSubString(typeStr, tmpTypeStr, tmpTypeDefStr);
    //                    }
    //                    pTemplateTypeDefs.insert(std::make_pair(typeStr, typeDefStr.value()));
    //                }
    //            }
    //        }
    //        return std::nullopt;
    //    }
    //    default: return std::nullopt;
    //    }
    //}
}