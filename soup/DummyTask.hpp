#pragma once

#include "Task.hpp"

namespace soup
{
	// To keep an otherwise-unoccupied scheduler running.
	struct DummyTask : public Task
	{
		void onTick() final
		{
		}
	};
}
