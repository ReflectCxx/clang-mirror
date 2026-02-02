#include <iostream>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <unordered_set>

#include "Logger.h"
#include "ASTCodeBuffer.h"
#include "ASTCodePrinter.h"
#include "ASTCodeManager.h"


namespace cxx 
{
    ASTCodeManager::ASTCodeManager()
    {
    }

    ASTCodeManager::~ASTCodeManager() 
    {
        for (auto& itr : m_codeGens) {
            delete itr.second;
        }
    }

    ASTCodeManager& ASTCodeManager::instance()
    {
        static ASTCodeManager instance;
        return instance;
    }

    void ASTCodeManager::setOutDir(const std::string& pOutDir)
    {
        m_outPath = pOutDir;
    }

    void ASTCodeManager::compilationFailedFor(const std::string& pSrcFile)
    {
        auto codeBuffer = getCodeBuffer(pSrcFile);
        if (codeBuffer) {
            codeBuffer->m_errorsFound = true;
        }
    }

    std::filesystem::path ASTCodeManager::getOutDir()
    {
        static auto dumpDir = [&]() {

            std::error_code errc;
            std::filesystem::path dumpDir = std::filesystem::path(m_outPath) / "rtl" / "cxxmirror";
            std::filesystem::create_directories(dumpDir, errc);
            if (errc) {
                Logger::outException("Failed to create output directory: " + errc.message());
                std::abort();
            }
            return dumpDir;
        }();
        return dumpDir;
    }

    void ASTCodeManager::dumpMetadataIds(std::fstream& pOut)
    {
        pOut << "\n#pragma once"
                "\n#include <string_view>\n"
                "\nnamespace " + std::string(NS_CXX) + " {\n";

        for (const auto& itr : m_codeGens) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrinter::printFreeFunctionIds(itr.second->getFreeFunctionsMap(), pOut);
            }
        }
        for (const auto& itr : m_codeGens) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrinter::printRecordTypeIds(itr.second->getRecordsMap(), pOut);
            }
        }
        pOut << "\n}";
    }

    void ASTCodeManager::dumpRegistrationDecls(std::fstream& pOut)
    {
        pOut << "\n#pragma once"
                "\n#include <vector>\n"
                "\nnamespace " + std::string(NS_CXX) + " { class Function; }\n"
                "\nnamespace " + std::string(NS_CXX) + " {\n"
                "\nnamespace " + std::string(NS_REGISTRATION) + " {"
                "\n    " + std::string(DECL_INIT_REGIS) + "\n}\n";

        for (const auto& itr : m_codeGens) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrinter::printRegistrationDecls(itr.second->getRecordsMap(), pOut);
            }
        }
        pOut << "\n}";
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


    void ASTCodeManager::dumpRegistrations(const std::string& pSrcFile, std::size_t pIndex)
    {
        auto codeBuffer = getCodeBuffer(pSrcFile);
        if (codeBuffer && !codeBuffer->isCompilationFailed()) 
        {
            auto fspath = getOutDir() / (std::string(FILE_REG_PREFIX) + std::to_string(pIndex) + ".cpp");
            std::fstream fout(fspath, std::ios::out);
            if (!fout.is_open()) {
                Logger::outException("Error opening file for writing!");
                return;
            }

            fout << "\n"
                    "\n#include \"" << std::string(FILE_REG_IDS) << "\""
                    "\n#include \"" << std::string(FILE_REG_DECLS) << "\"";

            fout.flush();
            fout.close();

            if (fout.fail() || fout.bad()) {
                Logger::outException("Error closing file:" + std::string(FILE_REG_IDS));
                return;
            }
            Logger::outgen(fspath.string());
        }
    }


    void ASTCodeManager::dumpCxxMirror()
    {
        {
            auto fpath = getOutDir() / std::string(FILE_REG_IDS);
            std::fstream fout(fpath, std::ios::out);
            if (!fout.is_open()) {
                Logger::outException("Error opening file for writing!");
                return;
            }

            dumpMetadataIds(fout);
            fout.flush();
            fout.close();

            if (fout.fail() || fout.bad()) {
                Logger::outException("Error closing file:" + std::string(FILE_REG_IDS));
                return;
            }
            Logger::outgen(fpath.string());
        } 
        {
            auto fpath = getOutDir() / std::string(FILE_REG_DECLS);
            std::fstream fout(fpath, std::ios::out);
            if (!fout.is_open()) {
                Logger::outException("Error opening file for writing!");
                return;
            }

            dumpRegistrationDecls(fout);
            fout.flush();
            fout.close();

            if (fout.fail() || fout.bad()) {
                Logger::outException("Error closing file:" + std::string(FILE_REG_IDS));
                return;
            }
            Logger::outgen(fpath.string());
        }
        Logger::out("Number of reflectable entities generated: " + std::to_string(m_codeGens.size()));
    }
}
