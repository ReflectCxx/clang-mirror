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

    template<EmitKind EK>
    std::filesystem::path ASTCodeManager::getOutDir()
    {
        static auto dumpDir = [&]() 
        {
            std::filesystem::path dumpDir;
            if constexpr (EK == EmitKind::CxxMirrorH || EK == EmitKind::CxxMirrorCpp) {
                dumpDir = std::filesystem::path(m_outPath) / NS_RTL;
            }
            else {
                dumpDir = std::filesystem::path(m_outPath) / NS_RTL / NS_CXX;
            }

            std::error_code errc;
            std::filesystem::create_directories(dumpDir, errc);
            if (errc) {
                Logger::outException("Failed to create output directory: " + errc.message());
                std::abort();
            }
            return dumpDir;
        }();
        return dumpDir;
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

    void ASTCodeManager::emitCxxMirrorHeader(std::fstream& pOut)
    {
        std::string incrtl = "rtl/rtl_access.h";
        std::string inccxx = std::string(NS_CXX).append("/").append(FILE_REG_IDS_H);

        pOut << "\n#pragma once\n"
                "\n#include \"" << incrtl << "\""
                "\n#include \"" << inccxx << "\"\n"
                "\nnamespace rtl { class CxxMirror; }"
                "\nnamespace cxx { static const rtl::CxxMirror& mirror(); }";
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
            auto fspath = getOutDir<EmitKind::RegistrationInitCpp>() / fname;
            std::fstream fout(fspath, std::ios::out);
            if (!fout.is_open()) {
                Logger::outException("Error opening file:" + fspath.string());
                return;
            }

            fout << "\n"
                    "\n#include \"" << std::string(FILE_REG_IDS_H) << "\""
                    "\n#include \"" << std::string(FILE_REG_INIT_H) << "\"";

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
        using EmiterT = std::function<void(ASTCodeManager&, std::fstream&)>;

        auto fwrite = [this]<EmitKind EK>(EmiterT emit, std::string_view file)
        {
            auto fspath = getOutDir<EK>() / file;
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

        fwrite.template operator() <EmitKind::RegistrationIdH> (&ASTCodeManager::emitMetadataIds, FILE_REG_IDS_H);
        fwrite.template operator() <EmitKind::RegistrationInitH> (&ASTCodeManager::emitRegistrationDecls, FILE_REG_INIT_H);
        fwrite.template operator() <EmitKind::CxxMirrorH> (&ASTCodeManager::emitCxxMirrorHeader, FILE_CXX_MIRROR_H);

        Logger::out("Number of reflectable entities generated: " + std::to_string(m_codeGens.size()));
    }
}
