#pragma once

#include "SchedulerAwareTask.hpp"

namespace soup
{
	struct SchedulerStats : public SchedulerAwareTask
	{
		size_t num_workers;
		size_t num_sockets;

		SchedulerStats(Scheduler& sched)
			: SchedulerAwareTask(sched)
		{
		}

		void onTick() final
		{
			num_workers = getScheduler().getNumWorkers();
			num_sockets = getScheduler().getNumSockets();
		}
	};
}
