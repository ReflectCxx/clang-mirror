
#pragma once

#include <string>


namespace nsdate
{
	struct Date
	{
		Date() = default;
		Date(Date&&) = default;
		Date(const Date&) = default;
		Date& operator=(Date&&) = default;
		Date& operator=(const Date&) = default;

		Date(const std::string& pDateStr);

		void set(std::string pDateStr);

		unsigned day() const { return m_day; }
		unsigned month() const { return m_day; }
		unsigned year() const { return m_day; }

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