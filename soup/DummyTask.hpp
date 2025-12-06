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

		int getSchedulingDisposition() const noexcept final
		{
			return LOW_FREQUENCY;
		}
	};
}
