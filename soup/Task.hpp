#pragma once

#include "Worker.hpp"

namespace soup
{
	struct Task : public Worker
	{
		explicit Task()
			: Worker()
		{
			holdup_type = Worker::IDLE;
			holdup_callback.set([](Worker& w, Capture&&)
			{
				reinterpret_cast<Task&>(w).tick();
			});
		}

		virtual void tick() = 0;
	};
}
