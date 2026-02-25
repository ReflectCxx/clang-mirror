
#include "Constants.h"
#include "Logger.h"
#include "StringUtils.h"
#include "ASTDeclsUtils.h"
#include "clang/AST/RecursiveASTVisitor.h"

using namespace clang;

namespace clmr 
{
    void ASTDeclsUtils::polishTypeStr(std::string& pTypeStr)
    {
        StringUtils::replaceSubString(pTypeStr, "std::basic_string_view<char>", "std::string_view");
        StringUtils::replaceSubString(pTypeStr, "std::basic_string<char>", "std::string");
        StringUtils::replaceSubString(pTypeStr, "_Bool", "bool");
        //StringUtils::replaceSubString(pTypeStr, " &&", "&&");
        //StringUtils::replaceSubString(pTypeStr, " &", "&");
        //StringUtils::replaceSubString(pTypeStr, " *", "*");
    }

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
        if (!method) return {};

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


    std::optional<std::string> ASTDeclsUtils::resolveHeaderFromDecl(const NamedDecl* pDecl,
                                                                    const SourceManager& pSrcMgr,
                                                                    const ClangPPCallbacks& pPP)
    {
        if (!pDecl) {
            return std::nullopt;
        }

        const auto& includeMap = pPP.getIncludeStrMap();
        for (auto* decl : pDecl->redecls())
        {
            SourceLocation loc = pSrcMgr.getSpellingLoc(decl->getLocation());
            if (!loc.isValid() || pSrcMgr.isInMainFile(loc)) continue;

            FileID fid = pSrcMgr.getFileID(loc);
            const FileEntry* fentry = pSrcMgr.getFileEntryForID(fid);
            if (!fentry) continue;

            auto itr = includeMap.find(fentry);
            if (itr != includeMap.end()) {
                return itr->second;
            }
        }
        return std::nullopt;
    }


    std::optional<std::string> ASTDeclsUtils::resolveHeaderFromType(const QualType& pQT,
                                                                    const ASTContext& pContext,
                                                                    const ClangPPCallbacks& pPP)
    {
        if (pQT.isNull()) {
            return std::nullopt;
        }

        const SourceManager& SM = pContext.getSourceManager();
        auto QT = pQT.getNonReferenceType();

        if (const auto* TST = QT->getAs<TemplateSpecializationType>()) {
            if (const TemplateDecl* TD = TST->getTemplateName().getAsTemplateDecl()){
                return resolveHeaderFromDecl(TD, SM, pPP);
            }
            Logger::outDbg("[skip] (TemplateSpecializationType) " + QT.getAsString());
            return std::nullopt;
        }

        if (const TypedefType* TT = QT->getAs<TypedefType>()) {
            return resolveHeaderFromDecl(TT->getDecl(), SM, pPP);
        }

        if (const TagType* TT = QT->getAs<TagType>()) {
            return resolveHeaderFromDecl(TT->getDecl(), SM, pPP);
        }

        if (QT->isBuiltinType()) {
            Logger::outDbg("[skip] (BuiltinType) " + QT.getAsString());
            return std::nullopt;
        }
        Logger::outDbg("[skip] (unknown) " + QT.getAsString());
        return std::nullopt;
    }
	

    std::string ASTDeclsUtils::extractQualifiedTypeName(const clang::QualType& pQType)
    {
        std::unordered_map<std::string, std::string> templateArgsTypeDefs;
        auto typedefStrValue = getTypeDefAliasForType(pQType, templateArgsTypeDefs);
        if (typedefStrValue.has_value())
        {
            std::string typedefOrgTypeKey;
            const auto& qt = pQType.getCanonicalType().getNonReferenceType();
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
        auto typeStr = pQType.getCanonicalType().getAsString();
        StringUtils::removeSubStrings(typeStr, { ENUM, CLASS, STRUCT });
        for (const auto& itr : templateArgsTypeDefs)
        {
            const auto& tmpTypeStr = itr.first;
            const auto& tmpTypeDefStr = itr.second;
            StringUtils::replaceSubString(typeStr, tmpTypeStr, tmpTypeDefStr);
        }
        polishTypeStr(typeStr);
        return typeStr;
    }


    std::optional<std::string> ASTDeclsUtils::getTypeDefAliasForType(const QualType& pQType,
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
}