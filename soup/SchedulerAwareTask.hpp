#pragma once

#include "Task.hpp"

namespace soup
{
	struct SchedulerAwareTask : public Task
	{
		SchedulerAwareTask(Scheduler& sched) noexcept
			: Task()
		{
			taskCapture().operator =<Scheduler*>(&sched);
		}

		[[nodiscard]] Scheduler& getScheduler() const noexcept
		{
			return *taskCapture().get<Scheduler*>();
		}
	};
}