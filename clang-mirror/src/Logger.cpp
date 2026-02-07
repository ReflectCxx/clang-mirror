
#include <iostream>
#include <sstream>
#include <iomanip>
#include <mutex>

#include "Constants.h"
#include "Logger.h"

namespace {
    static  std::mutex g_mutex;
}

namespace clmr {

    std::size_t Logger::m_totalCount = 0;
    std::size_t Logger::m_currentCount = 0;
    std::size_t Logger::m_totalDigitCount = 0;

    void Logger::resetDoneCounter(const int pTotalCount)
    {
        m_currentCount = 0;
        m_totalCount = pTotalCount;
        m_totalDigitCount = std::to_string(m_totalCount).length();
    }

    std::string Logger::formatProgress()
    {
        std::stringstream doneStream;
        std::stringstream progressStream;
        doneStream << std::setw(m_totalDigitCount) << std::setfill('0') << m_currentCount;
        progressStream << doneStream.str() << "/" << m_totalCount;
        return progressStream.str();
    }
}


namespace clmr {

    void Logger::out(const std::string& pMsg)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::cout << color::GREY << "\n[clang-mirror]\t" << color::RESET << pMsg;
    }

    void Logger::outgen(const std::string& pMsg)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::cout << color::GREY << "\n[clang-mirror]\t" << color::TEAL << "generated: " << color::GREY << pMsg;
    }

    void Logger::outException(const std::string& pMsg)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::cout << color::DARK_RED << "\n\t\t[exception] " << pMsg;
    }

    void Logger::outProgress(const std::string& pMsg, bool pUpdate/* = true*/)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (pUpdate) {
            m_currentCount++;
        }
        std::cout << color::GREEN << "\n[" << formatProgress() << "]\t" << color::TEAL << "compiling: " << color::RESET << pMsg;
    }

    void Logger::outError(const std::string& pSrcFile, const std::vector<std::string>& pUnreflectedFuncs, const std::vector<ErrorTuple>& pErrors)
    {
        std::lock_guard<std::mutex> lock(g_mutex);

        if (pErrors.empty() && pUnreflectedFuncs.empty()) {
            return;
        }

        std::cout << color::DARK_RED << "\n\t\t[error]" << color::GREY << " errors while compiling: " << color::CYAN << pSrcFile << color::GREY << ",";
        for (const auto& tuple : pErrors)
        {
            std::string missingMsg = std::get<2>(tuple);
            std::size_t start = missingMsg.find('\'');
            std::size_t end = missingMsg.find('\'', start + 1);
            if (start != std::string::npos && end != std::string::npos && end > start)
            {
                std::string quotedText = missingMsg.substr(start + 1, end - start - 1);
                std::string restOfMsg = missingMsg.substr(end + 1);
                std::cout << color::DARK_RED << "\n\t\t[error]\t"
                          << color::GREY << std::get<0>(tuple) << color::CYAN << std::get<1>(tuple)
                          << color::RESET << ": " << color::RED << "\'" << quotedText << "\'" << color::GREY << restOfMsg;
            }
            else
            {
                std::cout << color::DARK_RED << "\n\t\t[error]\t"
                          << color::GREY << std::get<0>(tuple) << color::CYAN << std::get<1>(tuple)
                          << color::RESET << ": " << color::RED << std::get<2>(tuple);
            }
        }

        for (const auto& func : pUnreflectedFuncs)
        {
            std::cout << color::DARK_RED << "\n\t\t[error]" << color::GREY << " unable to reflect: "
                      << color::YELLOW << func << "()";
        }
    }
}