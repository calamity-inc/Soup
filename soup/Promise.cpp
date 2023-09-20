#include "Promise.hpp"

#include <thread>

namespace soup
{
	void PromiseBase::awaitCompletion()
	{
		while (isPending())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	void Promise<void>::awaitCompletion()
	{
		while (isPending())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
