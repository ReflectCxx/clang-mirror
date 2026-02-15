

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

	bool Date::set(const std::string_view dateStr)
	{
		auto firstPos  = dateStr.find('/');
		auto secondPos = dateStr.find('/', firstPos + 1);
		
		if (firstPos == std::string_view::npos ||
			secondPos == std::string_view::npos ||
			dateStr.find('/', secondPos + 1) != std::string_view::npos) {
			return false;
		}

		auto dayStr   = dateStr.substr(0, firstPos);
		auto monthStr = dateStr.substr(firstPos + 1, secondPos - firstPos - 1);
		auto yearStr  = dateStr.substr(secondPos + 1);

		m_day   = std::stoi(std::string(dayStr));
		m_month = std::stoi(std::string(monthStr));
		m_year  = std::stoi(std::string(yearStr));
		return true;
	}
}