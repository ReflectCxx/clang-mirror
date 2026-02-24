
#include <set>
#include <vector>
#include <string>

#include "Logger.h"
#include "ClangDriver.h"
#include "ASTCodeManager.h"
#include "ASTParser.h"

#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/TargetParser/Host.h"

#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/CompilationDatabase.h"

#include "clang-tidy/ClangTidyOptions.h"
#include "clang-tidy/ClangTidyDiagnosticConsumer.h"

using namespace llvm;
using namespace clang::tidy;
using namespace clang::tooling;

namespace 
{
    static cl::OptionCategory g_clangMirrorCategory("clang-mirror options");

    static cl::opt<std::string> g_outDir(
        "out-dir",
        cl::desc("Directory where generated RTL registration code will be written"),
        cl::value_desc("path"),
        cl::cat(g_clangMirrorCategory)
    );

    static cl::opt<std::string> g_cdbDir(
        "cdb-dir",
        cl::desc("Directory containing the compile_commands.json"),
        cl::value_desc("path"),
        cl::cat(g_clangMirrorCategory)
    );

    static cl::list<std::string> g_excludeDirs(
        "exclude-dirs",
        cl::desc("Directories to exclude (comma separated or repeated flag)"),
        cl::ZeroOrMore,
        cl::CommaSeparated,
        cl::cat(g_clangMirrorCategory)
    );

    static cl::list<std::string> g_excludeNamespaces(
        "exclude-namespaces",
        cl::desc("Namespaces to exclude (comma separated or repeated flag)"),
        cl::ZeroOrMore,
        cl::CommaSeparated,
        cl::cat(g_clangMirrorCategory)
    );
}


namespace clmr
{
    void ClangDriver::collectSrcFiles(std::set<std::string>& pSrcSet, const std::vector<std::string>& pSrcFiles)
    {
        for (const auto& file : pSrcFiles)
        {
            bool skip = false;
            for (const auto& excStr : g_excludeDirs) {
                if (file.find(excStr) != std::string::npos) {
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                pSrcSet.insert(file);
            }
        }
    }


    bool ClangDriver::runClangParser(const std::vector<std::string>& pSrcFiles, CompilationDatabase& pCdb)
    {
        const int fileCount = pSrcFiles.size();
        Logger::out("Number of source files to process: " + std::to_string(fileCount));

        if (fileCount != 0) {
            Logger::resetDoneCounter(fileCount);
            ASTParser cxxParser(pSrcFiles);
            return cxxParser.parseFiles(pCdb, 0, fileCount - 1);
        }
        else {
            Logger::outError("no source files to process!");
            return false;
        }
    }
    

    bool ClangDriver::compileSourceFiles(int p_argc, const char** p_argv)
    {
        InitLLVM X(p_argc, p_argv);
        SmallVector<const char*> args{ p_argv, p_argv + p_argc };

        const bool isWin = Triple(sys::getProcessTriple()).isOSWindows();
        cl::TokenizerCallback Tokenizer = isWin ? cl::TokenizeWindowsCommandLine
                                                : cl::TokenizeGNUCommandLine;
        BumpPtrAllocator alloc;
        cl::ExpansionContext ectx(alloc, Tokenizer);
        if (Error err = ectx.expandResponseFiles(args)) {
            WithColor::error() << toString(std::move(err)) << "\n";
            return false;
        }

        p_argc = static_cast<int>(args.size());
        p_argv = args.data();

        auto optionsParser = CommonOptionsParser::create(p_argc, p_argv, g_clangMirrorCategory, cl::ZeroOrMore);
        if (!optionsParser) {
            llvm::WithColor::error() << llvm::toString(optionsParser.takeError());
            Logger::out("Failed to initialize CommonOptionsParser.");
            return false;
        }

        if (g_outDir.empty()) {
            llvm::WithColor::error() << "error: --out-dir is required\n";
            return false;
        }

        std::unique_ptr<CompilationDatabase> cdb;
        if (!g_cdbDir.empty()) {
            std::string errStr;
            cdb = std::move(CompilationDatabase::loadFromDirectory(g_cdbDir, errStr));
            if (!cdb) {
                Logger::outError(errStr);
            }
        }

        std::set<std::string> srcs;
        const auto& files = optionsParser->getSourcePathList();
        if (!files.empty()) {
            collectSrcFiles(srcs, files);
        }
        else if (cdb) {
            const auto& cdbSrcs = cdb->getAllFiles();
            collectSrcFiles(srcs, cdbSrcs);
        }

        ASTCodeManager::instance().setOutDir(g_outDir);
        ASTCodeManager::instance().setExcludeNamespaces({ g_excludeNamespaces.begin(), g_excludeNamespaces.end() });

        return runClangParser({ srcs.begin(), srcs.end() },
                              (cdb ? *cdb : optionsParser->getCompilations()));
    }
}