
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Constants.h"
#include "Logger.h"
#include "ClangDriver.h"
#include "ASTCodeManager.h"

//#include "E:\RTL\clang-mirror\SrcSamples\rtl\cxx_mirror.h"
//#include "E:\RTL\clang-mirror\SrcSamples\rtl\clmr\reg_decls.h"
//#include "E:\RTL\clang-mirror\SrcSamples\rtl\clmr\reg_ids.h"

int main(int argc, const char** argv)
{
    const auto& tsBegin = clmr::Clock::now();
    auto success = clmr::ClangDriver::compileSourceFiles(argc, argv);
    if (success){
        clmr::ASTCodeManager::instance().emitCxxMirror();
    }
    else{
        clmr::Logger::outException("error running clang-mirror! check logs for more details.\n");
    }

    const auto& tsEnd = std::chrono::duration_cast<clmr::Second> (clmr::Clock::now() - tsBegin).count();
    clmr::Logger::out("Total time elapsed: " + std::to_string(tsEnd) + "\n");

    return 0;
}