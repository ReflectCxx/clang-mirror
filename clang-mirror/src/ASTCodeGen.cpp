
#include "ASTCodeGen.h"
#include "ASTCodeBuffer.h"
#include "ASTCodePrinter.h"
#include "ASTCodeManager.h"

namespace clmr
{
	void ASTCodeGen::emitRegistrationInitsSource(std::ofstream& pOut, ASTCodeBuffer* pCodeBuffer)
	{
        pOut << "\n"
                "\n#include \"" << std::string(File::nameIDsHeader) << "\""
                "\n#include \"" << std::string(File::nameRegHeader) << "\""
                "\n"
                "\n";

        ASTCodePrint::outFunctionInitsDefs(pCodeBuffer->getFreeFunctionsMap(), pOut);
        ASTCodePrint::outTypeRecordInitDefs(pCodeBuffer->getRecordsMap(), pOut);
	}


    void ASTCodeGen::emitCxxMirrorHeader(std::ofstream& pOut, ASTCodeBuffer*)
    {
        std::string incIds = std::string(File::dirClmr).append("/").append(File::nameIDsHeader);

        pOut << "\n#pragma once\n"
                "\n#include \"" << File::incRtlAccess << "\""
                "\n#include \"" << incIds << "\"\n"
                "\nnamespace rtl { class CxxMirror; }"
                "\nnamespace cxx { static const rtl::CxxMirror& mirror(); }";
    }


    void ASTCodeGen::emitRegistrationInitsHeader(std::ofstream& pOut, ASTCodeBuffer*)
    {
        pOut << std::string("\n#pragma once"
                            "\n#include <vector>\n"
                            "\nnamespace " + std::string(NS_RTL) +
                            " { class Function; }\n\n");

        const auto& cbufs = ASTCodeManager::instance().getCodeBuffers();
        for (const auto& itr : cbufs) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outFreeFnsDecls(itr.second->getFreeFunctionsMap(), pOut);
            }
        }
        for (const auto& itr : cbufs) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outRecordInitDecls(itr.second->getRecordsMap(), pOut);
            }
        }
    }


    void ASTCodeGen::emitRegisteredIDsHeader(std::ofstream& pOut, ASTCodeBuffer*)
    {
        pOut << "\n#pragma once"
                "\n#include <string_view>\n"
                "\nnamespace " + std::string(NS_CXX) + " {\n";

        const auto& cbufs = ASTCodeManager::instance().getCodeBuffers();
        for (const auto& itr : cbufs) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outRegisteredFunctionIDs(itr.second->getFreeFunctionsMap(), pOut);
            }
        }
        for (const auto& itr : cbufs) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outRegisteredTypeRecordIDs(itr.second->getRecordsMap(), pOut);
            }
        }
        pOut << "\n}";
    }


	void ASTCodeGen::emitCxxMirrorSource(std::ofstream& pOut, ASTCodeBuffer*)
	{
        auto rtlHeader = File::incRtlBuilder;
        auto regIDHeader = std::string(File::dirClmr).append("/").append(File::nameRegHeader);

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
