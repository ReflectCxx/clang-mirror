
#include <iostream>
#include <algorithm>

#include "Constants.h"
#include "StringUtils.h"
#include "ASTCodeManager.h"
#include "ASTCodeBuffer.h"
#include "ASTDeclsUtils.h"
#include "CLMirrorASTVisitor.h"

#include "clang/AST/Type.h"
#include "clang/AST/PrettyPrinter.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

//namespace {
//
//    static std::string getQualifiedName(const clang::NamedDecl* pNDecl)
//    {
//        return pNDecl->getQualifiedNameAsString();
//    }
//
//    static std::string extractTypeAsString(clang::QualType pQType, const clang::ASTContext& pContext)
//    {
//        // Handle named record / enum types
//        if (const auto* RT = pQType->getAs<clang::RecordType>()) {
//            return getQualifiedName(RT->getDecl());
//        }
//        if (const auto* ET = pQType->getAs<clang::EnumType>()) {
//            return getQualifiedName(ET->getDecl());
//        }
//
//        // Handle typedefs / using-aliases
//        if (const auto* TT = pQType->getAs<clang::TypedefType>()) {
//            return getQualifiedName(TT->getDecl());
//        }
//        if (const auto* AT = pQType->getAs<clang::UsingType>()) {
//            return getQualifiedName(AT->getFoundDecl());
//        }
//
//        // Handle pointers / references / cv by recursion
//        if (pQType->isPointerType()) {
//            return extractTypeAsString(pQType->getPointeeType(), pContext) + "*";
//        }
//        if (pQType->isReferenceType()) {
//            return extractTypeAsString(pQType->getPointeeType(), pContext) +
//                (pQType->isLValueReferenceType() ? "&" : "&&");
//        }
//
//        // Handle const / volatile
//        if (pQType.isConstQualified()) {
//            return "const " +
//                extractTypeAsString(pQType.getUnqualifiedType(), pContext);
//        }
//        if (pQType.isVolatileQualified()) {
//            return "volatile " +
//                extractTypeAsString(pQType.getUnqualifiedType(), pContext);
//        }
//
//        // Handle template specializations
//        if (const auto* TST = pQType->getAs<clang::TemplateSpecializationType>()) {
//            const auto* TD = TST->getTemplateName().getAsTemplateDecl();
//            std::string result = getQualifiedName(TD);
//
//            result += "<";
//
//            const auto& tmplTypes = TST->template_arguments();
//
//            for (unsigned i = 0; i < tmplTypes.size(); ++i) {
//                const auto& Arg = tmplTypes[i];
//                if (Arg.getKind() == clang::TemplateArgument::Type) {
//                    result += extractTypeAsString(Arg.getAsType(), pContext);
//                }
//                if (i < (tmplTypes.size() - 1))
//                    result += ", ";
//            }
//            result += ">";
//            return result;
//        }
//
//        // Fallback for builtin types, function types, etc.
//        clang::PrintingPolicy policy(pContext.getLangOpts());
//        policy.adjustForCPlusPlus();
//        policy.FullyQualifiedName = true;
//        policy.SuppressUnwrittenScope = false;
//
//        std::string out;
//        llvm::raw_string_ostream os(out);
//        pQType.print(os, policy);
//        return out;
//    }
//}

namespace clmr
{
    CLMirrorASTVisitor::CLMirrorASTVisitor(const std::string& pSrcFile)
        : m_srcFile(pSrcFile)
	{ }


    //std::string CLMirrorASTVisitor::extractTypeAsString(clang::QualType pQType, const clang::ASTContext& pContext)
    //{
    //    clang::PrintingPolicy policy(pContext.getLangOpts());
    //    policy.adjustForCPlusPlus();

    //    policy.FullyQualifiedName = true;
    //    policy.SuppressScope = false;
    //    policy.SuppressTagKeyword = false;
    //    policy.SuppressUnwrittenScope = false;
    //    policy.PrintAsCanonical = false;
    //    policy.Bool = true;

    //    std::string typeStr;
    //    llvm::raw_string_ostream os(typeStr);
    //    pQType.print(os, policy);
    //    os.flush();

    //    llvm::errs() << "Type: ";
    //    pQType.print(llvm::errs(), policy);
    //    llvm::errs() << "\n";

    //    return typeStr;
    //}


    bool CLMirrorASTVisitor::VisitFunctionDecl(FunctionDecl* pFnDecl)
    {
        if (!ASTDeclsUtils::isInUserCode(pFnDecl) ||
            pFnDecl->isDeleted() ||
            pFnDecl->isInAnonymousNamespace() ||
            (pFnDecl->isGlobal() && pFnDecl->isStatic()) ||
            pFnDecl->isOverloadedOperator() ||
            pFnDecl->getKind() == Decl::Kind::CXXDestructor ||
            pFnDecl->getAccess() == AS_private ||
            pFnDecl->getAccess() == AS_protected ||
            pFnDecl->getLinkageInternal() != Linkage::External) {
            return true;
        }

        if (!pFnDecl->isThisDeclarationADefinition()) {
            return true;
        }

        if (pFnDecl->getFirstDecl() == nullptr) {
            return true;
        }

        if (!ASTDeclsUtils::isDeclFrmCurrentSource(m_srcFile, pFnDecl)) {
            return true;
        }

        std::string headerStr;
        auto& SM = pFnDecl->getASTContext().getSourceManager();
        for (auto* D : pFnDecl->redecls())
        {
            SourceLocation loc = SM.getSpellingLoc(D->getLocation());
            if (!loc.isValid()) {
                continue;
            }

            StringRef fileName = SM.getFilename(loc);
            if (fileName.ends_with(".h") || fileName.ends_with(".hpp"))
            {
                headerStr = fileName.str();
                break;
            }
        }

        if (!headerStr.empty()) 
        {
            std::vector<std::string> parmTypes;
            const auto& params = pFnDecl->parameters();
            const auto& fnQName = pFnDecl->getQualifiedNameAsString();
            for (unsigned index = 0; index < params.size(); index++) {
                parmTypes.push_back(ASTDeclsUtils::extractParameterType(params[index]));
            }

            std::string functionName;
            MetaKind metaKind = MetaKind::None;

            if (llvm::isa<clang::CXXConstructorDecl>(pFnDecl))
            {
                metaKind = MetaKind::Ctor;
                functionName = pFnDecl->getDeclName().getAsString();
            }
            else if (const auto* method = llvm::dyn_cast<clang::CXXMethodDecl>(pFnDecl))
            {
                if (method->isStatic()) {
                    metaKind = MetaKind::MemberFnStatic;
                }
                else {
                    if (method->isConst()) {
                        metaKind = MetaKind::MemberFnConst;
                    }
                    else {
                        metaKind = MetaKind::MemberFnNonConst;
                    }
                }
                functionName = pFnDecl->getDeclName().getAsString();
            }
            else {
                metaKind = MetaKind::NonMemberFn;
                functionName = pFnDecl->getQualifiedNameAsString();
            }

            auto codeBuffer = ASTCodeManager::instance().getCodeBuffer(m_srcFile, true);

            const std::string returnStr = pFnDecl->getReturnType().getAsString();
                //extractTypeAsString(pFnDecl->getReturnType(), pFnDecl->getASTContext());
            const std::string recordStr = ASTDeclsUtils::extractParentTypeName(pFnDecl);

            codeBuffer->addFunction({ 
                    .metaKind = metaKind,
                    .header = headerStr,
                    .record = recordStr,
                    .function = functionName
            }, returnStr, StringUtils::getParamTypesStr(parmTypes));
        }
        return true;
    }
}