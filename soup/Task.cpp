#include "Task.hpp"

//#include <thread>

#include "base.hpp"

#define LOG_TICK_DUR false

#if LOG_TICK_DUR
#include "log.hpp"
#include "format.hpp"
#include "Stopwatch.hpp"
#endif

namespace soup
{
	Task::Task() noexcept
		: Worker()
	{
		holdup_type = Worker::IDLE;
		holdup_callback.set([](Worker& w, Capture&&)
		{
			reinterpret_cast<Task&>(w).onTick();
		});
	}

	void Task::tick()
	{
		if (!isWorkDone())
		{
			onTick();
		}
	}

	bool Task::tickUntilDone()
	{
		SOUP_IF_UNLIKELY (isWorkDone())
		{
			return true;
		}
		onTick();
		return isWorkDone();
	}

	void Task::runUntilDone()
	{
#if LOG_TICK_DUR
		Stopwatch t;
		while (t.start(), !tickUntilDone())
#else
		while (!tickUntilDone())
#endif
		{
#if LOG_TICK_DUR
			t.stop();
			logWriteLine(format("Tick took {} ms", t.getMs()));
#endif
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
