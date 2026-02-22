
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

    static cl::opt<std::string> outDir(
        "out-dir",
        cl::desc("Directory where generated RTL registration code will be written"),
        cl::value_desc("path"),
        cl::cat(g_clangMirrorCategory)
    );

    static cl::opt<std::string> cdbDir(
        "cdb-dir",
        cl::desc("Directory containing the compile_commands.json"),
        cl::value_desc("path"),
        cl::cat(g_clangMirrorCategory)
    );
}


namespace clmr
{
    bool ClangDriver::runClangParser(const std::vector<std::string>& pSrcFiles, CompilationDatabase& pCdb)
    {
        const int fileCount = pSrcFiles.size();
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

        if (outDir.empty()) {
            llvm::WithColor::error() << "error: --out-dir is required\n";
            return false;
        }

        ASTCodeManager::instance().setOutDir(outDir);
        // For definite ordering, so the registration namespace creation on different platform remains same.
        std::set<std::string> distinctSrcs;
        if(cdbDir.empty())
        {
            std::string cdbLoadErr;
            StringRef cdbPathStr;
            const auto& files = optionsParser->getSourcePathList();
            if (!files.empty()) {
                cdbPathStr = files.front();
            }
            distinctSrcs.insert(files.begin(), files.end());
        }
        else
        {
            std::string errStr;
            auto cdb = CompilationDatabase::loadFromDirectory(cdbDir, errStr);
            if(cdb) {
                const auto& files = cdb->getAllFiles();
                distinctSrcs.insert(files.begin(), files.end());
            }
            else {
                Logger::outError(errStr);
                return false;
            }
        }

        Logger::out("Number of source files to process: " + std::to_string(distinctSrcs.size()));
        const auto& finalSrcFiles = std::vector<std::string>(distinctSrcs.begin(), distinctSrcs.end());
        return runClangParser(finalSrcFiles, optionsParser->getCompilations());
    }
}