
#pragma once

#include <string>

namespace nsdate
{
	struct Date
	{
		Date();
		Date(const Date& pOther);
		Date(const std::string& pDateStr);
		Date(unsigned dd, unsigned mm, unsigned yy);
		Date(Date&&) noexcept;

		Date& operator=(Date&&) = default;
		Date& operator=(const Date&) = default;

		const bool operator==(const Date& pOther) const;

		~Date();

		static std::size_t instanceCount();

		std::string getAsString() const;

		void updateDate(std::string pDateStr);

	private:

		unsigned m_day;
		unsigned m_month;
		unsigned m_year;
		static std::size_t m_instanceCount;
	};
}
