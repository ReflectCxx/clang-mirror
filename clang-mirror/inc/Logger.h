#pragma once

#include "Constants.h"

namespace clmr {

    class Logger
    {
        static std::size_t m_totalCount;
        static std::size_t m_currentCount;
        static std::size_t m_totalDigitCount;

        static std::string formatProgress();

    public:

        static void out(const std::string& pMsg);
        static void outgen(const std::string& pMsg);
        static void outError(const std::string& pMsg);
        static void resetDoneCounter(const int pTotalCount);
        static void outProgress(const std::string& pMsg, bool pUpdate = true);
        static void outDbg(const std::string& pMsg, RegErr pErr = RegErr::None);
	};
}