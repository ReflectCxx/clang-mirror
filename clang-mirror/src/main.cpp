
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Constants.h"
#include "Logger.h"
#include "ClangDriver.h"
#include "ASTCodeManager.h"

int main(int argc, const char** argv)
{
    auto begin = clmr::Clock::now();
    auto success = clmr::ClangDriver::compileSourceFiles(argc, argv);
    if (success){
        clmr::ASTCodeManager::instance().emitCxxMirror();
    }
    else{
        clmr::Logger::outException("error running clang-mirror! check logs for more details.\n");
    }

    auto end = std::chrono::duration_cast<clmr::Second> (clmr::Clock::now() - begin).count();
    clmr::Logger::out("Total time elapsed: " + std::to_string(end) + "\n");

    return 0;
}