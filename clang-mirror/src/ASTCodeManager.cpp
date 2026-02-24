

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

    void ASTCodeManager::setExcludeNamespaces(const std::vector<std::string>& pExcludeNs)
    {
        m_excludeNamespaces = pExcludeNs;
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

    void ASTCodeManager::collectGlobalFunctions(CxxFunctionsMap& pGlobalFns)
    {
        for (auto& itr : m_codeBuffs) 
        {
            if (itr.second->isCompilationFailed()) {
                continue;
            }
            for (const auto& [fname, fnMeta] : itr.second->getFreeFunctionsMap()) {

                auto [it, inserted] = pGlobalFns.emplace(fname, fnMeta);
                if (!inserted) {
                    it->second.signatures.push_back(fnMeta.signatures.back());
                }
            }
        }
    }

    bool ASTCodeManager::emitCxxMirror()
    {
        using CGen = ASTCodeGen;
        using CMgr = ASTCodeManager;

        if( dump(File::nameIDsHeader, &CMgr::toRootDir, &CGen::emitRegisteredIDsHeader) &&
            dump(File::nameRegHeader, &CMgr::toRootDir, &CGen::emitRegistrationInitsHeader) ){
            Logger::out("Registered entities from " + std::to_string(m_codeBuffs.size()) + " source files.");
            
            if( dump(File::nameCxxHeader, &CMgr::toRootDir, &CGen::emitCxxMirrorHeader) && 
                dump(File::nameCxxSource, &CMgr::toSrcDir, &CGen::emitCxxMirrorSource) ){
                auto pathStr = toRootDir(m_outPath).string();
                Logger::out("Registration code generated in: " + pathStr);
                return true;
            }
            else {
                Logger::out("Failed generating cxx_mirror interface.");
            }
        }
        else {
            Logger::out("Failed generating IDs & init() headers.");
        }
        return false;
    }


    bool ASTCodeManager::emitRegistrationSource(const std::string& pSrcFile, std::size_t pIndex)
    {
        auto codeBuffer = getCodeBuffer(pSrcFile);
        if (codeBuffer && !codeBuffer->isCompilationFailed()) {

            std::string fname = std::string(File::prefixRegs) + std::to_string(pIndex) + "_";
            fname.append(std::filesystem::path(pSrcFile).stem().string())
                 .append(".cpp");

            codeBuffer->setSrcFileIndex(pIndex);
            return dump(fname, &ASTCodeManager::toSrcDir, &ASTCodeGen::emitRegistrationInitsSource, codeBuffer);
        }
        return false;
    }


    std::filesystem::path ASTCodeManager::toSrcDir(std::string_view pPath)
    {
        static auto dir = [&]() {
            std::error_code err;
            std::filesystem::path dir = std::filesystem::path(pPath) / File::dirRoot / File::dirSrc;
            std::filesystem::create_directories(dir, err);
            if (err) {
                Logger::outError("Failed to create output directory: " + err.message());
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
                Logger::outError("Failed to create output directory: " + err.message());
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


    bool ASTCodeManager::dump(std::string_view pFile, GetDir pGetDir, Emitter pEmiter,
                              ASTCodeBuffer* pCb /*= nullptr*/)
    {
        std::filesystem::path fspath = pGetDir(m_outPath) / pFile;
        std::filesystem::path temp = fspath;
        temp += ".tmp";

        std::ofstream fout(temp);
        if (!fout) {
            Logger::outError("Error opening file: " + fspath.string());
            return false;
        }

        pEmiter(fout, pCb);

        if (!fout) {
            fout.close();
            std::filesystem::remove(temp);
            Logger::outError("Error writing file: " + fspath.string());
            return false;
        }
        fout.close();

        std::error_code ec;
        std::filesystem::remove(fspath, ec);      // Required, `rename` does not replace existing file on Windows.
        std::filesystem::rename(temp, fspath, ec);
        if (ec) {
            Logger::outError("Error replacing file: " + fspath.string());
            return false;
        }
        Logger::outgen(fspath.string());
        return true;
    }
}