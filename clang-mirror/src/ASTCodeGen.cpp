
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
                            "\n#include \"" + std::string(File::incRtlBuilder) + "\""
                            "\n\n");

        const auto& cbuffs = ASTCodeManager::instance().getCodeBufferMap();
        for (const auto& itr : cbuffs) {
            const auto& cb = *itr.second;
            if (!cb.isCompilationFailed() && !cb.getFreeFunctionsMap().empty()) {
                ASTCodePrint::outFreeFnsDecls(pOut, cb.getSrcFileIndex());
            }
        }
        for (const auto& itr : cbuffs) 
        {
            const auto& cb = *itr.second;
            auto srcIndex = cb.getSrcFileIndex();
            if (!cb.isCompilationFailed()) {
                for (const auto& itr : cb.getRecordsMap()) {
                    ASTCodePrint::outRecordInitDecls(pOut, srcIndex, itr.second.typeIndex);
                }
            }
        }
    }


    void ASTCodeGen::emitRegisteredIDsHeader(std::ofstream& pOut, ASTCodeBuffer*)
    {
        pOut << "\n#pragma once"
                "\n#include <array>"
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
        std::string varName = "fns";
        std::string regIDHeader = std::string("../").append(File::nameRegHeader);

        pOut << "\n"
                "\n#include <vector>"
                "\n"
                "\n#include \"" << regIDHeader << "\""
                "\n"
                "\nnamespace cxx { \n"
                "\n    const rtl::CxxMirror& mirror()"
                "\n    {"
                "\n        static auto mirror = rtl::CxxMirror([]() {"
                "\n"
                "\n            std::vector<rtl::Function> "<< varName <<";\n";

        auto& codeBuffers = ASTCodeManager::instance().getCodeBufferMap();
        for (auto& itr : codeBuffers)
        {   
            auto& cbuf = *itr.second;
            if (!cbuf.getFreeFunctionsMap().empty())
            {
                std::string codeStr = "\n            ";
                codeStr.append(NS_REGS).append(std::to_string(cbuf.getSrcFileIndex()))
                       .append("::").append(NS_FN)
                       .append("::").append(REGIS_FN_INIT)
                       .append("(").append(varName).append(");");
                pOut << codeStr;
            }
        }

        for (auto& itr : codeBuffers)
        {   
            auto& cbuf = *itr.second;
            if (!cbuf.getRecordsMap().empty())
            {
                const auto& srcIndexStr = std::to_string(cbuf.getSrcFileIndex());
                for (auto& itr0 : cbuf.getRecordsMap())
                {
                    const auto& typIndexStr = std::to_string(itr0.second.typeIndex);
                    std::string codeStr = "\n            ";
                    codeStr.append(NS_REGS).append(srcIndexStr)
                           .append("::").append(NS_TYPE).append(typIndexStr)
                           .append("::").append(REGIS_FN_INIT)
                           .append("(").append(varName).append(");");
                    pOut << codeStr;
                }
            }
        }

        pOut << "\n            return " << varName <<";"
                "\n        }());"
                "\n        return mirror;"
                "\n    }"
                "\n}";
	}
}