
#include "Constants.h"
#include "Logger.h"
#include "ClangDriver.h"
#include "ASTCodeManager.h"

int main(int argc, const char** argv)
{
    auto begin = clmr::Clock::now();

    if (!clmr::ClangDriver::compileSourceFiles(argc, argv) ||
        !clmr::ASTCodeManager::instance().emitCxxMirror()) 
    {
        clmr::Logger::outError("errors occurred while generating registration code!");
    }

    auto end = std::chrono::duration_cast<clmr::Second> (clmr::Clock::now() - begin).count();
    clmr::Logger::out("Total time elapsed: " + std::to_string(end));
    return 0;
}