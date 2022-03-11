#include "worker.hpp"

#include "promise.hpp"

namespace soup
{
	void worker::operator=(worker&& b) noexcept
	{
		holdup_type = b.holdup_type;
		holdup_callback = std::move(b.holdup_callback);
		holdup_data = b.holdup_data;
	}

	void worker::fireHoldupCallback()
	{
		holdup_type = NONE;
		holdup_callback(*this);
	}

	void worker::awaitPromiseCompletion(promise_base* p, void(*f)(worker&, capture&&), capture&& cap)
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
}
