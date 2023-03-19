#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "SchedulerAwareTask.hpp"

#include <optional>

#include "DelayedCtor.hpp"
#include "HttpRequest.hpp"
#include "netConnectTask.hpp"
#include "Uri.hpp"

namespace soup
{
	struct HttpRequestTask : public SchedulerAwareTask
	{
		HttpRequest hr;
		netConnectTask connector;
		bool connecting = true;
		Socket* sock;
		std::optional<HttpResponse> res; // Output

		HttpRequestTask(Scheduler* sched, const Uri& uri)
			: SchedulerAwareTask(sched), hr(uri), connector(sched, hr.getHost(), hr.port)
		{
		}

		void onTick() final;

		void sendRequest();
	};
}

#endif
