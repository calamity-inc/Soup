#pragma once

#include "Scheduler.hpp"
#if !SOUP_WASM

#include "netConfig.hpp"
#include "Thread.hpp"

namespace soup
{
	class DetachedScheduler : public Scheduler
	{
	protected:
		netConfig conf;
	public:
		Thread thrd;

		DetachedScheduler(netConfig&& conf = {});

		SharedPtr<Worker> addWorker(SharedPtr<Worker>&& w) noexcept final;

		[[nodiscard]] bool isActive() const noexcept;

		void updateConfig(void fp(netConfig&, Capture&&), Capture&& cap = {});

		void closeReusableSockets();

	protected:
		void run();

		virtual void onPreRun();
		virtual void onPostRun();
	};
}

#endif
