
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Constants.h"
#include "Logger.h"
#include "ClangDriver.h"
#include "ASTCodeManager.h"

//#include "E:\RTL\clang-mirror\TestProjectSrc\rtl\cxxmirror\rtl_registered_ids.h"

int main(int argc, const char** argv)
{
    const auto& tsBegin = clmirror::Clock::now();

    if (clmirror::ClangDriver::compileSourceFiles(argc, argv))
    {
        clmirror::ASTCodeManager::instance().dumpCxxMirror();
    }
    else
    {
        clmirror::Logger::outException("error running clang-mirror! check logs for more details.\n");
    }

    std::cout << clmirror::RESET << std::flush;
    const auto& tsEnd = std::chrono::duration_cast<clmirror::Second> (clmirror::Clock::now() - tsBegin).count();
    clmirror::Logger::out("Total time elapsed: " + std::to_string(tsEnd) + "\n");

    //auto id = cxx::type::nsdate::Date::fn::Date::id;

    return 0;
}