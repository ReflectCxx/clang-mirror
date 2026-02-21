
#include <iostream>
#include <sstream>
#include <iomanip>

#include "Constants.h"
#include "Logger.h"

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
        std::cout << color::GREY << "\n[clang-mirror]\t" << color::RESET << pMsg;
    }

    void Logger::outgen(const std::string& pMsg)
    {
        std::cout << color::GREY << "\n[clang-mirror]\t" << color::TEAL << "generated: " << color::GREY << pMsg;
    }

    void Logger::outError(const std::string& pMsg)
    {
        std::cout << color::DARK_RED << "\n[clang-mirror]\t" << pMsg;
    }

    void Logger::outProgress(const std::string& pMsg, bool pUpdate/* = true*/)
    {
        if (pUpdate) {
            m_currentCount++;
        }
        std::cout << color::GREEN << "[" << formatProgress() << "]\t" << color::TEAL << "compiling: " << color::RESET << pMsg << std::endl;
    }
}