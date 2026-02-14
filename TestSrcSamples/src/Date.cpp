
#include <vector>
#include "Date.h"

namespace nsdate 
{
	std::string getDateAsString(Date date)
	{
		return (std::to_string(date.day()) + "/" + std::to_string(date.month()) + "/" + std::to_string(date.year()));
	}

	std::string getDateAsString(unsigned dd, unsigned mm, unsigned yy)
	{
		return (std::to_string(dd) + "/" + std::to_string(mm) + "/" + std::to_string(yy));
	}

	void Date::set(std::string pDateStr)
	{
		std::string strBuf;
		std::vector<std::string> date;
		for (size_t i = 0; i < pDateStr.length(); i++)
		{
			if (pDateStr[i] == '/') {
				date.push_back(strBuf);
				strBuf.clear();
			}
			else {
				strBuf.push_back(pDateStr[i]);
			}
		}
		m_day = stoi(date[0]);
		m_month = stoi(date[1]);
		m_year = stoi(strBuf);
	}
}