#include "Task.hpp"

#include "UniquePtr.hpp"

namespace soup
{
	Task::Task()
		: Worker()
	{
		holdup_type = Worker::IDLE;
		holdup_callback.set([](Worker& w, Capture&&)
		{
			reinterpret_cast<Task&>(w).tick();
		});
	}

	void Task::awaitTaskCompletion(UniquePtr<Task>&& task)
	{
		holdup_type = Worker::TASK;
		holdup_callback.cap = std::move(task);
	}
}
