#pragma once

#include "Task.hpp"

#include "Scheduler.hpp"

namespace soup
{
	struct SchedulerStats : public Task
	{
		size_t num_workers;
		size_t num_sockets;

		SchedulerStats()
		{
		}

		void onTick() final
		{
			num_workers = Scheduler::get()->getNumWorkers();
			num_sockets = Scheduler::get()->getNumSockets();
		}
	};
}
