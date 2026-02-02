
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Constants.h"
#include "Logger.h"
#include "ClangDriver.h"
#include "ASTCodeManager.h"

//#include "E:/RTL/clang-mirror/TestProjectSrc/rtl/cxxmirror/rtl_registered_ids.h"

int main(int argc, const char** argv)
{
    const auto& tsBegin = cxx::Clock::now();

    if (cxx::ClangDriver::compileSourceFiles(argc, argv))
    {
        cxx::ASTCodeManager::instance().dumpCxxMirror();
    }
    else
    {
        cxx::Logger::outException("error running clang-mirror! check logs for more details.\n");
    }

    std::cout << cxx::RESET << std::flush;
    const auto& tsEnd = std::chrono::duration_cast<cxx::Second> (cxx::Clock::now() - tsBegin).count();
    cxx::Logger::out("Total time elapsed: " + std::to_string(tsEnd) + "\n");

    //cxx::type::Animal::fn::setAnimalName::

    return 0;
}