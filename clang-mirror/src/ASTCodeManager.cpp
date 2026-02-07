

#include <mutex>
#include <fstream>
#include <iostream>
#include <functional>
#include <filesystem>
#include <unordered_set>

#include "Logger.h"
#include "ASTCodeGen.h"
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
        using CGen = ASTCodeGen;
        using CMgr = ASTCodeManager;

        dump(File::nameIDsHeader, &CMgr::toRootDir, &CGen::emitRegisteredIDsHeader);
        dump(File::nameRegHeader, &CMgr::toRootDir, &CGen::emitRegistrationInitsHeader);
        dump(File::nameCxxHeader, &CMgr::toRootDir, &CGen::emitCxxMirrorHeader);
        dump(File::nameCxxSource, &CMgr::toSrcDir, &CGen::emitCxxMirrorSource);

        Logger::out("Registered entities from " + std::to_string(m_codeBuffs.size()) + " source files.");
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

            codeBuffer->setSrcFileIndex(pIndex);
            dump(fname, &ASTCodeManager::toSrcDir, &ASTCodeGen::emitRegistrationInitsSource, codeBuffer);
        }
    }


    std::filesystem::path ASTCodeManager::toSrcDir(std::string_view pPath)
    {
        static auto dir = [&]() {
            std::error_code err;
            std::filesystem::path dir = std::filesystem::path(pPath) / File::dirRoot / File::dirSrc;
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
            std::filesystem::path dir = std::filesystem::path(pPath) / File::dirRoot;
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
        auto it = m_codeBuffs.find(pSrcFile);
        if (it != m_codeBuffs.end()) {
            return it->second.get();
        }
        if (!pCreate) {
            return nullptr;
        }

        auto uptr = std::make_unique<ASTCodeBuffer>(pSrcFile);
        auto codeBuff = uptr.get();
        m_codeBuffs.emplace(pSrcFile, std::move(uptr));
        return codeBuff;
    }


    void ASTCodeManager::dump(std::string_view pFile, GetDir pGetDir, Emitter pEmiter,
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

        pEmiter(fout, pCodeBuffer);

        if (!fout) {
            fout.close();
            std::filesystem::remove(temp);
            Logger::outException("Error writing file: " + fspath.string());
            return;
        }
        fout.close();

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