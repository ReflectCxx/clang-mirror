
#include "ASTCodeGen.h"
#include "ASTCodeBuffer.h"
#include "ASTCodePrinter.h"
#include "ASTCodeManager.h"

namespace clmr
{
	void ASTCodeGen::emitRegistrationCpp(std::ofstream& pOut, ASTCodeBuffer* pCodeBuffer)
	{
        pOut << "\n"
                "\n#include \"" << std::string(File::nameIDsHeader) << "\""
                "\n#include \"" << std::string(File::nameRegHeader) << "\""
                "\n"
                "\n";

        ASTCodePrint::outFreeFnsInitDefs(pCodeBuffer->getFreeFunctionsMap(), pOut);
        ASTCodePrint::outRecordInitDefs(pCodeBuffer->getRecordsMap(), pOut);
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


    void ASTCodeGen::emitRegistrationFns(std::ofstream& pOut, ASTCodeBuffer*)
    {
        pOut << std::string("\n#pragma once"
                            "\n#include <vector>\n"
                            "\nnamespace " + std::string(NS_RTL) +
                            " { class Function; }\n\n");

        const auto& cbuffers = ASTCodeManager::instance().getCodeBuffers();
        for (const auto& itr : cbuffers) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outFreeFnsDecls(itr.second->getFreeFunctionsMap(), pOut);
            }
        }
        for (const auto& itr : cbuffers) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outRecordInitDecls(itr.second->getRecordsMap(), pOut);
            }
        }
    }


    void ASTCodeGen::emitRegisteredIds(std::ofstream& pOut, ASTCodeBuffer*)
    {
        pOut << "\n#pragma once"
                "\n#include <string_view>\n"
                "\nnamespace " + std::string(NS_CXX) + " {\n";

        const auto& cbuffers = ASTCodeManager::instance().getCodeBuffers();
        for (const auto& itr : cbuffers) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outFreeFunctionIDs(itr.second->getFreeFunctionsMap(), pOut);
            }
        }
        for (const auto& itr : cbuffers) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outTypeRecordIDs(itr.second->getRecordsMap(), pOut);
            }
        }
        pOut << "\n}";
    }


	void ASTCodeGen::emitCxxMirrorSource(std::ofstream& pOut, ASTCodeBuffer*)
	{
		std::string incDecls = std::string(File::dirClmr).append("/").append(File::nameRegHeader);

        pOut << "\n"
                "\n#include <vector>"
                "\n"
                "\n#include \"" << File::incRtlBuilder << "\""
                "\n#include \"" << incDecls << "\""
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
