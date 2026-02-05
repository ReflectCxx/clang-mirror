

#include <iostream>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <functional>

#include "Logger.h"
#include "ASTCodeBuffer.h"
#include "ASTCodePrinter.h"
#include "ASTCodeManager.h"


namespace clmr {

    void ASTCodeManager::setOutDir(const std::string& pOutDir)
    {
        m_outPath = pOutDir;
    }

    ASTCodeManager& ASTCodeManager::instance()
    {
        static ASTCodeManager instance;
        return instance;
    }

    void ASTCodeManager::compilationFailedFor(const std::string& pSrcFile)
    {
        auto codeBuffer = getCodeBuffer(pSrcFile);
        if (codeBuffer) {
            codeBuffer->setErrorsFound(true);
        }
    }

    void ASTCodeManager::emitCxxMirror()
    {
        using CMgr = ASTCodeManager;
        dump(&CMgr::emitRegisteredIds, &CMgr::toClmrDir, File::nameIDsHeader);
        dump(&CMgr::emitRegistrationFns, &CMgr::toClmrDir, File::nameRegHeader);
        dump(&CMgr::emitCxxMirrorHeader, &CMgr::toRootDir, File::nameCxxHeader);
        dump(&CMgr::emitCxxMirrorSource, &CMgr::toRootDir, File::nameCxxSource);

        Logger::out("Registered entities from " + std::to_string(m_codeGens.size()) + " source files.");
    }


    void ASTCodeManager::emitRegistrationSource(const std::string& pSrcFile, std::size_t pIndex)
    {
        auto codeBuffer = getCodeBuffer(pSrcFile);
        if (codeBuffer && !codeBuffer->isCompilationFailed()) {

            std::string fname = std::string(File::prefixRegs);
            fname.append(std::to_string(pIndex))
                 .append("_")
                 .append(std::filesystem::path(pSrcFile).stem().string())
                 .append(".cpp");

            dump(&ASTCodeManager::emitRegistrationCpp, &ASTCodeManager::toClmrDir, fname, codeBuffer);
        }
    }


    std::filesystem::path ASTCodeManager::toClmrDir(std::string_view pPath)
    {
        static auto dir = [&]() {
            std::error_code err;
            std::filesystem::path dir = std::filesystem::path(pPath) / File::dirRtl / File::dirClmr;
            std::filesystem::create_directories(dir, err);
            if (err) {
                Logger::outException("Failed to create output directory: " + err.message());
                std::abort();
            }
            return dir;
        }();
        return dir;
    }


    std::filesystem::path ASTCodeManager::toRootDir(std::string_view pPath)
    {
        static auto dir = [&]() {
            std::error_code err;
            std::filesystem::path dir = std::filesystem::path(pPath) / File::dirRtl;
            std::filesystem::create_directories(dir, err);
            if (err) {
                Logger::outException("Failed to create output directory: " + err.message());
                std::abort();
            }
            return dir;
        }();
        return dir;
    }


    ASTCodeBuffer* ASTCodeManager::getCodeBuffer(const std::string& pSrcFile, bool pCreate /*= false*/)
    {
        auto it = m_codeGens.find(pSrcFile);
        if (it != m_codeGens.end()) {
            return it->second.get();
        }
        if (!pCreate) {
            return nullptr;
        }

        auto uptr = std::make_unique<ASTCodeBuffer>(pSrcFile);
        auto codeBuff = uptr.get();
        m_codeGens.emplace(pSrcFile, std::move(uptr));
        return codeBuff;
    }


    void ASTCodeManager::dump(Emitter pEmiter, GetDir pGetDir, std::string_view pFile,
                              ASTCodeBuffer* pCodeBuffer /*= nullptr*/)
    {
        std::filesystem::path fspath = pGetDir(m_outPath) / pFile;
        std::filesystem::path temp = fspath;
        temp += ".tmp";

        std::ofstream fout(temp);
        if (!fout) {
            Logger::outException("Error opening file: " + fspath.string());
            return;
        }

        (this->*pEmiter)(fout, pCodeBuffer);

        if (!fout) {
            fout.close();
            std::filesystem::remove(temp);
            Logger::outException("Error writing file: " + fspath.string());
            return;
        }
        fout.close(); // ensure buffers flushed

        std::error_code ec;
        std::filesystem::remove(fspath, ec);      // Required, `rename` does not replace existing file on Windows.
        std::filesystem::rename(temp, fspath, ec);
        if (ec) {
            Logger::outException("Error replacing file: " + fspath.string());
            return;
        }
        Logger::outgen(fspath.string());
    }
}



namespace clmr {

    void ASTCodeManager::emitRegistrationCpp(std::ofstream& pOut, ASTCodeBuffer* pCodeBuffer)
    {
        pOut << "\n"
                "\n#include \"" << std::string(File::nameIDsHeader) << "\""
                "\n#include \"" << std::string(File::nameRegHeader) << "\""
                "\n"
                "\n";

        ASTCodePrint::outFreeFnsInitDefs(pCodeBuffer->getFreeFunctionsMap(), pOut);
        ASTCodePrint::outRecordInitDefs(pCodeBuffer->getRecordsMap(), pOut);
    }


    void ASTCodeManager::emitRegistrationFns(std::ofstream& pOut, ASTCodeBuffer*)
    {
        pOut << std::string("\n#pragma once"
                            "\n#include <vector>\n"
                            "\nnamespace " + std::string(NS_RTL) +
                            " { class Function; }\n");
        pOut<< "\n";
        for (const auto& itr : m_codeGens) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outFreeFnsDecls(itr.second->getFreeFunctionsMap(), pOut);
            }
        }
        for (const auto& itr : m_codeGens) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outRecordInitDecls(itr.second->getRecordsMap(), pOut);
            }
        }
    }


    void ASTCodeManager::emitCxxMirrorHeader(std::ofstream& pOut, ASTCodeBuffer*)
    {
        std::string incIds = std::string(File::dirClmr).append("/").append(File::nameIDsHeader);

        pOut << "\n#pragma once\n"
                "\n#include \"" << File::incRtlAccess << "\""
                "\n#include \"" << incIds << "\"\n"
                "\nnamespace rtl { class CxxMirror; }"
                "\nnamespace cxx { static const rtl::CxxMirror& mirror(); }";
    }


    void ASTCodeManager::emitRegisteredIds(std::ofstream& pOut, ASTCodeBuffer*)
    {
        pOut << "\n#pragma once"
                "\n#include <string_view>\n"
                "\nnamespace " + std::string(NS_CXX) + " {\n";

        for (const auto& itr : m_codeGens) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outFreeFunctionIDs(itr.second->getFreeFunctionsMap(), pOut);
            }
        }
        for (const auto& itr : m_codeGens) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrint::outTypeRecordIDs(itr.second->getRecordsMap(), pOut);
            }
        }
        pOut << "\n}";
    }


    void ASTCodeManager::emitCxxMirrorSource(std::ofstream& pOut, ASTCodeBuffer*)
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