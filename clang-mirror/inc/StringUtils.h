
#pragma once

#include<string>
#include<vector>

namespace cxx {

    struct StringUtils 
    {
        static std::string getParamTypesStr(const std::vector<std::string>& pArgs)
        {
            std::string signStr;
            for (size_t i = 0; i < pArgs.size(); ++i) {
                if (i > 0) {
                    signStr += ", ";
                }
                signStr += pArgs[i];
            }
            return signStr;
        }

        static void replaceSubString(std::string& pSrcStr, const std::string& pSubstr, const std::string& pReplacestr)
        {
            if (pReplacestr.find(pSubstr) != std::string::npos) {
                return;
            }

            size_t pos = std::string::npos;
            do {
                pos = pSrcStr.find(pSubstr);
                if (pos != std::string::npos) {
                    pSrcStr.replace(pos, pSubstr.length(), pReplacestr);
                }
            } while (pos != std::string::npos);
        }

        static void removeSubStrings(std::string& pSrcStr, const std::vector<std::string_view>& pKeyStrs)
        {
            for (const auto& keyStr : pKeyStrs)
            {
                if (!keyStr.empty())
                {
                    size_t pos = pSrcStr.find(keyStr);
                    while (pos != std::string::npos)
                    {
                        size_t charCount = keyStr.size();
                        const size_t& endPos = (pos + charCount);
                        if (endPos < pSrcStr.size() && pSrcStr.at(endPos) == ' ') {
                            charCount++;
                        }
                        pSrcStr.erase(pos, charCount);
                        pos = pSrcStr.find(keyStr, pos);
                    }
                }
            }
        }

        static std::vector<std::string> splitQualifiedName(const std::string& qname)
        {
            std::vector<std::string> parts;
            size_t start = 0;
            while (true)
            {
                size_t pos = qname.find("::", start);
                if (pos == std::string::npos)
                {
                    parts.emplace_back(qname.substr(start));
                    break;
                }
                parts.emplace_back(qname.substr(start, pos - start));
                start = pos + 2;
            }
            return parts;
        }
    };
}