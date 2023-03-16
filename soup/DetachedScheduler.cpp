#include "DetachedScheduler.hpp"
#if !SOUP_WASM

namespace soup
{
	DetachedScheduler::DetachedScheduler()
		: thrd([](Capture&& cap)
		{
			cap.get<DetachedScheduler*>()->run();
		}, this)
	{
	}

	Worker& DetachedScheduler::addWorker(UniquePtr<Worker>&& w) noexcept
	{
		if (!thrd.isRunning())
		{
			thrd.start([](Capture&& cap)
			{
				cap.get<DetachedScheduler*>()->run();
			}, this);
		}
		return Scheduler::addWorker(std::move(w));
	}

	bool DetachedScheduler::isActive() const noexcept
	{
		return thrd.isRunning();
	}
}

#endif
