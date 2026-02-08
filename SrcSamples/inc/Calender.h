#pragma once

#include <memory>

namespace nsdate
{
	struct Date;
	struct Event;

	struct Calender
	{
		Calender();
		~Calender();
		Calender(Calender&&) noexcept;
		Calender(const Calender&);

		Calender& operator=(Calender&&) = delete;
		Calender& operator=(const Calender&) = delete;

		Date& getTheDate();
		Date& getSavedDate();

		const Event& getTheEvent();
		const Event& getSavedEvent();

		static void resetMoveOpsCounter();
		static std::size_t instanceCount();
		static std::size_t getMoveOpsCount();

		static Calender create();

	private:

		std::shared_ptr<Event> m_theEvent;

		std::unique_ptr<Event> m_savedEvent;

		static std::size_t m_instanceCount;

		static std::size_t m_moveOpsCount;
	};
}