#include "Worker.hpp"

#include "Promise.hpp"

namespace soup
{
	void Worker::operator=(Worker&& b) noexcept
	{
		holdup_type = b.holdup_type;
		holdup_callback = std::move(b.holdup_callback);
		holdup_data = b.holdup_data;
	}

	void Worker::fireHoldupCallback()
	{
		recursions = 0;
		holdup_type = NONE;
		holdup_callback(*this);
	}

	void Worker::awaitPromiseCompletion(PromiseBase* p, void(*f)(Worker&, Capture&&), Capture&& cap)
	{
		if (p->isPending())
		{
			holdup_type = PROMISE;
			holdup_callback.set(f, std::move(cap));
			holdup_data = p;
		}
		else
		{
			f(*this, std::move(cap));
		}
	}

	bool Worker::canRecurse() noexcept
	{
		return ++recursions != 20;
	}
}
