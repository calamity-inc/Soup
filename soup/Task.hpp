#pragma once

#include "Worker.hpp"

namespace soup
{
	struct Task : public Worker
	{
		explicit Task();

		virtual void tick() = 0;

		void awaitTaskCompletion(UniquePtr<Task>&& task);
	};
}
