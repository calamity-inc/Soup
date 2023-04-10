#pragma once

#include "Task.hpp"

namespace soup
{
	//template <typename Task = Task>
	struct StateMachineTask : public Task
	{
		using state_t = uint8_t;

		state_t state = 0;

		using Task::Task;

		void setState(state_t state)
		{
			this->state = state;
			return onTick();
		}
	};
}
