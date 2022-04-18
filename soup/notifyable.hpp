#pragma once

#include <condition_variable>
#include <mutex>

namespace soup
{
	struct notifyable : public std::condition_variable
	{
		void wait()
		{
			std::mutex m{};
			std::unique_lock<std::mutex> l(m);
			condition_variable::wait(l);
		}
	};
}
