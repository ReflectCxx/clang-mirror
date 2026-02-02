
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Constants.h"
#include "Logger.h"
#include "ClangDriver.h"
#include "ASTCodeManager.h"

int main(int argc, const char** argv)
{
    const auto& tsBegin = clmr::Clock::now();

    if (clmr::ClangDriver::compileSourceFiles(argc, argv))
    {
        clmr::ASTCodeManager::instance().createCxxMirror();
    }
    else
    {
        clmr::Logger::outException("error running clang-mirror! check logs for more details.\n");
    }

    std::cout << clmr::RESET << std::flush;
    const auto& tsEnd = std::chrono::duration_cast<clmr::Second> (clmr::Clock::now() - tsBegin).count();
    clmr::Logger::out("Total time elapsed: " + std::to_string(tsEnd) + "\n");

    return 0;
}