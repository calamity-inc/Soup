#pragma once

#include "Task.hpp"

#include "ObfusString.hpp"
#include "Scheduler.hpp"

NAMESPACE_SOUP
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

		std::string toString() const SOUP_EXCAL final
		{
			return ObfusString("SchedulerStats").str();
		}
	};
}
