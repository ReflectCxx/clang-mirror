
#pragma once

#include <memory>
#include "Date.h"

namespace nsdate
{
	struct Calender;

	struct Event
	{
		~Event();

		Event(Event&&) = delete;

		static std::size_t instanceCount();

		const Date& getEventDate();

		void reset();

	private:

		Event();
		Event(const Event& pOther);

		std::unique_ptr<Date> m_date;

		static std::size_t m_instanceCount;

		static Event* create();
		static Event* createCopy(const Event& pOther);

		//friends :)
		friend Calender;

		Event& operator=(Event&&) = delete;
		Event& operator=(const Event&) = delete;
	};
}