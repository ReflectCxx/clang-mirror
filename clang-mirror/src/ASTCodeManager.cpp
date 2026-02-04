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

    
    void ASTCodeManager::emitCxxMirror()
    {
        using CMgr = ASTCodeManager;

        dump(&CMgr::emitRegisteredIds, &CMgr::toCxxDir, File::nameIDsHeader);
        dump(&CMgr::emitRegistrationFns, &CMgr::toCxxDir, File::nameRegHeader);
        dump(&CMgr::emitCxxMirrorHeader, &CMgr::toRootDir, File::cxxMirHeader);
        Logger::out("Registered entities from " + std::to_string(m_codeGens.size()) + " source files.");
    }


    void ASTCodeManager::emitRegistrationSource(const std::string& pSrcFile, std::size_t pIndex)
    {
        using CMgr = ASTCodeManager;
        auto codeBuffer = getCodeBuffer(pSrcFile);
        if (codeBuffer && !codeBuffer->isCompilationFailed()) {

            std::string fname(File::regInitSrcPrefix);
            fname.append(std::to_string(pIndex)).append(".cpp");    
            dump(&CMgr::emitRegistrationsCpp, &CMgr::toCxxDir, fname);
        }
    }


    std::filesystem::path ASTCodeManager::toCxxDir(std::string_view pPath)
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


    std::filesystem::path ASTCodeManager::toRootDir(std::string_view pPath)
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


    void ASTCodeManager::emitCxxMirrorHeader(std::ofstream& pOut)
    {
        std::string incrtl = "rtl/rtl_access.h";
        std::string inccxx = std::string(NS_CXX).append("/").append(File::nameIDsHeader);

        pOut << "\n#pragma once\n"
                "\n#include \"" << incrtl << "\""
                "\n#include \"" << inccxx << "\"\n"
                "\nnamespace rtl { class CxxMirror; }"
                "\nnamespace cxx { static const rtl::CxxMirror& mirror(); }";
    }


    void ASTCodeManager::emitRegistrationsCpp(std::ofstream& pOut)
    {
        pOut << "\n"
                "\n#include \"" << std::string(File::nameIDsHeader) << "\""
                "\n#include \"" << std::string(File::nameRegHeader) << "\"";
    }


    void ASTCodeManager::emitRegisteredIds(std::ofstream& pOut)
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


    void ASTCodeManager::emitRegistrationFns(std::ofstream& pOut)
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

        (this->*pEmiter)(fout);

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