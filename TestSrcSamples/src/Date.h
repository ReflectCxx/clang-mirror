
#pragma once

#include <string>


namespace nsdate
{
	struct Date
	{
		bool set(const std::string_view pDateStr);

		unsigned day() const { return m_day; }
		unsigned month() const { return m_month; }
		unsigned year() const { return m_year; }

	private:

		unsigned m_day = 1;
		unsigned m_month = 1;
		unsigned m_year = 2020;
	};
}


namespace nsdate
{
	std::string getDateAsString(Date date);
	std::string getDateAsString(unsigned dd, unsigned mm, unsigned yy);
}