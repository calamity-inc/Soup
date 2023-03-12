#pragma once

#include "Worker.hpp"

namespace soup
{
	class Task : public Worker
	{
	public:
		explicit Task();
	protected:
		virtual void onTick() = 0;

	public:
		void tick();

		// For use within another Task's onTick. Returns true once done.
		[[nodiscard]] bool tickUntilDone();
	
		// To use a Task without a Scheduler
		void runUntilDone();
	};
}
