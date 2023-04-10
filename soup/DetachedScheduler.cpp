#include "DetachedScheduler.hpp"
#if !SOUP_WASM

namespace soup
{
	DetachedScheduler::DetachedScheduler(netConfig&& conf)
		: conf(std::move(conf)), thrd([](Capture&& cap)
		{
			cap.get<DetachedScheduler*>()->run();
		}, this)
	{
	}

	SharedPtr<Worker> DetachedScheduler::addWorker(SharedPtr<Worker>&& w) noexcept
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

	struct UpdateConfigTask : public Task
	{
		Callback<void(netConfig&)> cb;

		UpdateConfigTask(void fp(netConfig&, Capture&&), Capture&& cap)
			: cb(fp, std::move(cap))
		{
		}

		void onTick() final
		{
			cb(netConfig::get());
			setWorkDone();
		}
	};

	void DetachedScheduler::updateConfig(void fp(netConfig&, Capture&&), Capture&& cap)
	{
		if (isActive())
		{
			add<UpdateConfigTask>(fp, std::move(cap));
		}
		else
		{
			fp(conf, std::move(cap));
		}
	}

	struct CloseReusableSocketsTask : public Task
	{
		void onTick() final
		{
			Scheduler::get()->closeReusableSockets();
			setWorkDone();
		}
	};

	void DetachedScheduler::closeReusableSockets()
	{
		if (isActive())
		{
			add<CloseReusableSocketsTask>();
		}
	}

	void DetachedScheduler::run()
	{
		netConfig::get() = std::move(conf);
		onPreRun();
		Scheduler::run();
		onPostRun();
		workers.clear();
		passive_workers = 0;
		conf = std::move(netConfig::get());
	}

	void DetachedScheduler::onPreRun()
	{
	}

	void DetachedScheduler::onPostRun()
	{
	}
}

#endif
