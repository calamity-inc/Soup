#pragma once

#include "Worker.hpp"

namespace soup
{
	class Task : public Worker
	{
	public:
		explicit Task();

		void tick();
		[[nodiscard]] bool tickUntilDone();
	protected:
		virtual void onTick() = 0;
	};
}
