#include "Task.hpp"

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
}
