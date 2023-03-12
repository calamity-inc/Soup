#include "Task.hpp"

#include <thread>

#include "base.hpp"

namespace soup
{
	Task::Task()
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
		while (!tickUntilDone())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
