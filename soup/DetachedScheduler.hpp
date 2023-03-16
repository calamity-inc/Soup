#pragma once

#include "Scheduler.hpp"

#include "Thread.hpp"

namespace soup
{
	// Note: You cannot construct this on the stack because the thread needs a `this` pointer.
	class DetachedScheduler : public Scheduler
	{
	public:
		Thread thrd;

		DetachedScheduler();

		Worker& addWorker(UniquePtr<Worker>&& w) noexcept final;

		[[nodiscard]] bool isActive() const noexcept;
	};
}
