#pragma once

#include <vector>
#include <string>
#include "clang/Tooling/Tooling.h"

namespace clmr
{
    class ClangDriver
    {
        static void runClangParser(const std::vector<std::string>& pSrcFiles, clang::tooling::CompilationDatabase& pCdb);

    public:

        static bool compileSourceFiles(int argc, const char** argv);
    };
}