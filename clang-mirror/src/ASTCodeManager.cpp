

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

    ASTCodeManager::ASTCodeManager()
    { }

    ASTCodeManager::~ASTCodeManager() {
        for (auto& itr : m_codeGens) {
            delete itr.second;
        }
    }

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
            codeBuffer->m_errorsFound = true;
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

            dump(&ASTCodeManager::emitRegistrationCpp, &ASTCodeManager::toClmrDir, fname);
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
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);

        if (pCreate) {
            auto codeBuffer = [&]()-> ASTCodeBuffer*
            {
                const auto& itr = m_codeGens.find(pSrcFile);
                if (itr == m_codeGens.end()) 
                {
                    auto codeBuffer = new ASTCodeBuffer(pSrcFile);
                    m_codeGens.insert(std::make_pair(pSrcFile, codeBuffer));
                    return codeBuffer;
                }
                else {
                    auto& codeBuffer = itr->second;
                    return codeBuffer;
                }
            }();
            return codeBuffer;
        }
        else {
            const auto& itr = m_codeGens.find(pSrcFile);
            return (itr != m_codeGens.end() ? itr->second : nullptr);
        }
    }


    void ASTCodeManager::dump(Emitter pEmiter, GetDir pGetDir, std::string_view pFile)
    {
        std::filesystem::path fspath = pGetDir(m_outPath) / pFile;
        std::filesystem::path temp = fspath;
        temp += ".tmp";

        std::ofstream fout(temp);
        if (!fout) {
            Logger::outException("Error opening file: " + fspath.string());
            return;
        }

        bool success = (this->*pEmiter)(fout);

        if (!success || !fout) {
            fout.close();
            std::filesystem::remove(temp);
            if (success) {
                Logger::outException("Error writing file: " + fspath.string());
            }
            else {
                Logger::outException("Compilation error occured.");
            }
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

    bool ASTCodeManager::emitRegistrationFns(std::ofstream& pOut)
    {
        pOut << std::string("\n#pragma once"
                            "\n#include <vector>\n"
                            "\nnamespace " + std::string(NS_RTL) +
                            " { class Function; }\n");
        pOut<< "\n";
        for (const auto& itr : m_codeGens) {
            if (itr.second->isCompilationFailed()) {
                return false;
            }
            ASTCodePrint::outRegistrationDecls(itr.second->getFreeFunctionsMap(), pOut);
        }
        for (const auto& itr : m_codeGens) {
            if (itr.second->isCompilationFailed()) {
                return false;
            }
            ASTCodePrint::outRegistrationDecls(itr.second->getRecordsMap(), pOut);
        }
        return true;
    }


    bool ASTCodeManager::emitCxxMirrorHeader(std::ofstream& pOut)
    {
        std::string incIds = std::string(File::dirClmr).append("/").append(File::nameIDsHeader);

        pOut << "\n#pragma once\n"
                "\n#include \"" << File::incRtlAccess << "\""
                "\n#include \"" << incIds << "\"\n"
                "\nnamespace rtl { class CxxMirror; }"
                "\nnamespace cxx { static const rtl::CxxMirror& mirror(); }";
        return true;
    }


    bool ASTCodeManager::emitRegisteredIds(std::ofstream& pOut)
    {
        pOut << "\n#pragma once"
                "\n#include <string_view>\n"
                "\nnamespace " + std::string(NS_CXX) + " {\n";

        for (const auto& itr : m_codeGens) {
            if (itr.second->isCompilationFailed()) {
                return false;
            }
            ASTCodePrint::outFreeFunctions(itr.second->getFreeFunctionsMap(), pOut);
        }
        for (const auto& itr : m_codeGens) {
            if (itr.second->isCompilationFailed()) {
                return false;
            }
            ASTCodePrint::outTypeRecords(itr.second->getRecordsMap(), pOut);
        }
        pOut << "\n}";
        return true;
    }


    bool ASTCodeManager::emitRegistrationCpp(std::ofstream& pOut)
    {
        pOut << "\n"
                "\n#include \"" << std::string(File::nameIDsHeader) << "\""
                "\n#include \"" << std::string(File::nameRegHeader) << "\""
                "\n"
                "\n";

        for (const auto& itr : m_codeGens) {
            if (itr.second->isCompilationFailed()) {
                return false;
            }
            ASTCodePrint::outRegistrationDefns(itr.second->getFreeFunctionsMap(), pOut);
            
        }
        for (const auto& itr : m_codeGens) {
            if (itr.second->isCompilationFailed()) {
                return false;
            }
            ASTCodePrint::outRegistrationDefns(itr.second->getRecordsMap(), pOut);
        }
        return true;
    }


    bool ASTCodeManager::emitCxxMirrorSource(std::ofstream& pOut)
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
        return true;
    }
}