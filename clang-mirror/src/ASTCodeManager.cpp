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


namespace clmr
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


    std::filesystem::path ASTCodeManager::inCxxDir(std::string pPath)
    {
        static auto dir = [&]()
        {
            std::error_code err;
            std::filesystem::path dir = std::filesystem::path(pPath) / NS_RTL / NS_CXX;
            std::filesystem::create_directories(dir, err);
            if (err) {
                Logger::outException("Failed to create output directory: " + err.message());
                std::abort();
            }
            return dir;
        }();
        return dir;
    }


    std::filesystem::path ASTCodeManager::inRtlDir(std::string pPath)
    {
        static auto dir = [&]()
        {
            std::error_code err;
            std::filesystem::path dir = std::filesystem::path(pPath) / NS_RTL;
            std::filesystem::create_directories(dir, err);
            if (err) {
                Logger::outException("Failed to create output directory: " + err.message());
                std::abort();
            }
            return dir;
        }();
        return dir;
    }


    void ASTCodeManager::emitCxxMirrorHeader(std::fstream& pOut)
    {
        std::string incrtl = "rtl/rtl_access.h";
        std::string inccxx = std::string(NS_CXX).append("/").append(File_RegisteredIdsH);

        pOut << "\n#pragma once\n"
                "\n#include \"" << incrtl << "\""
                "\n#include \"" << inccxx << "\"\n"
                "\nnamespace rtl { class CxxMirror; }"
                "\nnamespace cxx { static const rtl::CxxMirror& mirror(); }";
    }


    void ASTCodeManager::emitMetadataIds(std::fstream& pOut)
    {
        pOut << "\n#pragma once"
                "\n#include <string_view>\n"
                "\nnamespace " + std::string(NS_CXX) + " {\n";

        for (const auto& itr : m_codeGens) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrinter::printFreeFunctions(itr.second->getFreeFunctionsMap(), pOut);
            }
        }
        for (const auto& itr : m_codeGens) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrinter::printTypeRecords(itr.second->getRecordsMap(), pOut);
            }
        }
        pOut << "\n}";
    }


    void ASTCodeManager::emitRegistrationDecls(std::fstream& pOut)
    {
        pOut << ASTCodePrinter::getIncludesForRegistrations() << "\n";

        for (const auto& itr : m_codeGens) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrinter::printRegistrationDecls(itr.second->getFreeFunctionsMap(), pOut);
            }
        }

        for (const auto& itr : m_codeGens) {
            if (!itr.second->isCompilationFailed()) {
                ASTCodePrinter::printRegistrationDecls(itr.second->getRecordsMap(), pOut);
            }
        }
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
            std::string fname = std::string(FILE_REG_PREFIX).append(std::to_string(pIndex))
                                                            .append(".cpp");
            auto fspath = inCxxDir(m_outPath) / fname;
            std::fstream fout(fspath, std::ios::out);
            if (!fout.is_open()) {
                Logger::outException("Error opening file:" + fspath.string());
                return;
            }

            fout << "\n"
                    "\n#include \"" << std::string(File_RegisteredIdsH) << "\""
                    "\n#include \"" << std::string(File_RegisteredInitH) << "\"";

            fout.flush();
            fout.close();

            if (fout.fail() || fout.bad()) {
                Logger::outException("Error closing file:" + fspath.string());
                return;
            }
            Logger::outgen(fspath.string());
        }
    }


    void ASTCodeManager::emitCxxMirror()
    {
        using DirT = std::function<std::filesystem::path(std::string)>;
        using EmiterT = std::function<void(ASTCodeManager&, std::fstream&)>;

        auto fwrite = [this](EmiterT emit, std::string_view file, DirT getDir)
        {
            auto fspath = getDir(m_outPath) / file;
            std::fstream fout(fspath, std::ios::out);
            if (!fout.is_open()) {
                Logger::outException("Error opening file:" + fspath.string());
                return;
            }

            emit(*this, fout);
            fout.flush();
            fout.close();

            if (fout.fail() || fout.bad()) {
                Logger::outException("Error closing file:" + fspath.string());
                return;
            }
            Logger::outgen(fspath.string());
        };

        fwrite(&ASTCodeManager::emitMetadataIds, File_RegisteredIdsH,
               &ASTCodeManager::inCxxDir);

        fwrite(&ASTCodeManager::emitRegistrationDecls, File_RegisteredInitH,
               &ASTCodeManager::inCxxDir);

        fwrite(&ASTCodeManager::emitCxxMirrorHeader, File_CxxMirrorH,
               &ASTCodeManager::inRtlDir);

        Logger::out("Number of reflectable entities generated: " + std::to_string(m_codeGens.size()));
    }
}