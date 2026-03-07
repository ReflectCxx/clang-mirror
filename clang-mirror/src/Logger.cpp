
#include <iostream>
#include <sstream>
#include <iomanip>

#include "Constants.h"
#include "Logger.h"

namespace {

    inline constexpr bool g_debugLog = false;
    inline constexpr std::string_view clang_mirror = "[clang-mirror]";

    std::string fmtNewlines(const std::string& pStr)
    {
        auto indent = clang_mirror.size();
        std::size_t slashnCount = 0;
        for (char ch : pStr) {
            if (ch == '\n') ++slashnCount;
        }

        std::string fmtStr;
        fmtStr.reserve(pStr.size() + slashnCount * (indent + 1));
        for (int i = 0; i < pStr.size(); i++) 
        {
            if (i != pStr.size() - 1) {
                fmtStr += pStr[i];
                if (pStr[i] == '\n') {
                    fmtStr.append(indent, ' ');
                    fmtStr +="\t";
                }
            }
            else if (pStr[i] != '\n') {
                fmtStr += pStr[i];
            }
        }
        return fmtStr;
    }
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
        std::cout << color::GREY << clang_mirror << "\t" << color::RESET << pMsg << std::endl;
    }

    void Logger::outgen(const std::string& pMsg)
    {
        std::cout << color::GREY << clang_mirror << "\t" << color::TEAL << "generated: " << color::GREY << pMsg << std::endl;
    }

    void Logger::outError(const std::string& pMsg)
    {
        std::cout << color::RED_DARK << clang_mirror << "\t" << fmtNewlines(pMsg) << color::RESET << std::endl;
    }
    
    void Logger::outDbg(const std::string &pMsg, RegErr pErr)
    {
        if(g_debugLog) {
            auto clang_spaces = std::string(clang_mirror.length(), ' ');

            auto errColor = (pErr == RegErr::HeaderNotFound ||
                             pErr == RegErr::AstParsing) ? color::RED : color::BLUE;

            if (errColor == color::BLUE) {
                errColor = (pErr == RegErr::ExclusionByPolicy ||
                            pErr == RegErr::HeaderNotPublic) ? color::GREEN : color::BLUE;
            }

            auto errCode = (pErr != RegErr::None ? (" [" + toString(pErr) + "] ") : "");
            std::cout << clang_spaces << "\t" << errColor << errCode << color::GREY << pMsg << color::RESET << std::endl;
        }
    }

    void Logger::outProgress(const std::string& pMsg, bool pUpdate/* = true*/)
    {
        if (pUpdate) {
            m_currentCount++;
        }
        std::cout << color::GREEN << "[" << formatProgress() << "]\t" << color::TEAL << "compiling: " << color::RESET << pMsg << std::endl;
    }
}