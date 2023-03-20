#pragma once

#include "Scheduler.hpp"
#if !SOUP_WASM

#include "netConfig.hpp"
#include "Thread.hpp"

namespace soup
{
	// Note: You cannot construct this on the stack because the thread needs a `this` pointer.
	class DetachedScheduler : public Scheduler
	{
	protected:
		netConfig conf;
	public:
		Thread thrd;

		DetachedScheduler(netConfig&& conf = {});

		Worker& addWorker(UniquePtr<Worker>&& w) noexcept final;

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
