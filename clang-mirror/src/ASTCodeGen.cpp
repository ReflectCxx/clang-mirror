
#include "ASTCodeGen.h"
#include "ASTCodeBuffer.h"
#include "ASTCodePrinter.h"
#include "ASTCodeManager.h"

namespace clmr
{
    void ASTCodeGen::emitCxxMirrorHeader(std::ofstream& pOut, ASTCodeBuffer*)
    {
        pOut << "\n#pragma once\n"
                "\n#include \"" << File::nameIDsHeader << "\""
                "\n#include \"" << File::incRtlAccess << "\"\n"
                "\nnamespace rtl { class CxxMirror; }"
                "\nnamespace cxx { extern const rtl::CxxMirror& mirror(); }";
    }


    void ASTCodeGen::emitRegistrationInitsHeader(std::ofstream& pOut, ASTCodeBuffer*)
    {
        pOut << std::string("\n#pragma once"
                            "\n#include <vector>\n"
                            "\nnamespace " + std::string(NS_RTL) +
                            " { class Function; }\n\n");

        const auto& cbuffs = ASTCodeManager::instance().getCodeBufferMap();
        for (const auto& itr : cbuffs) {
            const auto& cb = *itr.second;
            if (!cb.isCompilationFailed() && !cb.getFreeFunctionsMap().empty()) {
                ASTCodePrint::outFreeFnsDecls(pOut, cb.getSrcFileIndex());
            }
        }
        for (const auto& itr : cbuffs) {
            const auto& cb = *itr.second;
            if (!cb.isCompilationFailed() && !cb.getRecordsMap().empty()) {
                ASTCodePrint::outRecordInitDecls(pOut, cb.getSrcFileIndex());
            }
        }
    }


    void ASTCodeGen::emitRegisteredIDsHeader(std::ofstream& pOut, ASTCodeBuffer*)
    {
        pOut << "\n#pragma once"
                "\n#include <string_view>\n"
                "\nnamespace " + std::string(NS_CXX) + " {\n";

        const auto& cbuffs = ASTCodeManager::instance().getCodeBufferMap();
        for (const auto& itr : cbuffs) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outRegisteredFunctionIDs(itr.second->getFreeFunctionsMap(), pOut);
            }
        }
        for (const auto& itr : cbuffs) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outRegisteredTypeRecordIDs(itr.second->getRecordsMap(), pOut);
            }
        }
        pOut << "\n}";
    }


    void ASTCodeGen::emitRegistrationInitsSource(std::ofstream& pOut, ASTCodeBuffer* pCb)
    {
        std::string headerIds = std::string("../").append(File::nameIDsHeader);
        std::string headerInits = std::string("../").append(File::nameRegHeader);

        pOut << "\n"
                "\n#include \"" << headerIds << "\""
                "\n#include \"" << headerInits << "\"";

        const auto& includesSet = pCb->getIncludesSet();
        for (auto& incStr : includesSet) {
            pOut << "\n#include " << incStr;
        }
        pOut << "\n\n";

        ASTCodePrint::outFunctionInitsDefs(*pCb, pOut);
        ASTCodePrint::outTypeRecordInitsDefs(*pCb, pOut);
    }


	void ASTCodeGen::emitCxxMirrorSource(std::ofstream& pOut, ASTCodeBuffer*)
	{
        std::string rtlHeader = std::string("../").append(File::incRtlBuilder);
        std::string regIDHeader = std::string("../").append(File::nameRegHeader);

        pOut << "\n"
                "\n#include <vector>"
                "\n"
                "\n#include \"" << rtlHeader << "\""
                "\n#include \"" << regIDHeader << "\""
                "\n"
                "\nnamespace cxx { \n"
                "\n    const rtl::CxxMirror& mirror()"
                "\n    {"
                "\n        static auto mirror = rtl::CxxMirror([]() {"
                "\n"
                "\n            std::vector<rtl::Function> fns;"
                "\n"
                "\n        }());"
                "\n        return mirror;"
                "\n    }"
                "\n}";
	}
}
