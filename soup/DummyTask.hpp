#pragma once

#include "Task.hpp"

NAMESPACE_SOUP
{
	// To keep an otherwise-unoccupied scheduler running.
	struct DummyTask : public Task
	{
		void onTick() final
		{
		}
	};
}
