
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
}


namespace clmr
{
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

        Expected<CommonOptionsParser> optionsParser = 
            CommonOptionsParser::create(p_argc, p_argv, g_clangMirrorCategory, cl::ZeroOrMore);

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

        std::string cdbLoadErr;
        StringRef cdbPathStr;
        const auto& pathList = optionsParser->getSourcePathList();
        if (!pathList.empty()) {
            cdbPathStr = pathList.front();
        }

        // For definite ordering, so the registration namespace creation on different platform remains same.
        std::set<std::string> distinctSrcFiles(pathList.begin(), pathList.end());
        Logger::out("Number of source files to process: " + std::to_string(distinctSrcFiles.size()));
        const auto& finalSrcFiles = std::vector<std::string>(distinctSrcFiles.begin(), distinctSrcFiles.end());
        return runClangParser(finalSrcFiles, optionsParser->getCompilations());
    }


    bool ClangDriver::runClangParser(const std::vector<std::string>& pSrcFiles, CompilationDatabase& pCdb)
    {
        const int fileCount = pSrcFiles.size();
        Logger::resetDoneCounter(fileCount);
        ASTParser cxxParser(pSrcFiles);
        return cxxParser.parseFiles(pCdb, 0, fileCount - 1);
    }
}