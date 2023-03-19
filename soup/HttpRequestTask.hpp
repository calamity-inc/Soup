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
	class HttpRequestTask : public SchedulerAwareTask
	{
	public:
		enum State : uint8_t
		{
			WAIT_TO_REUSE,
			CONNECTING,
			AWAIT_RESPONSE,
		};

		State state;
		HttpRequest hr;
		DelayedCtor<netConnectTask> connector;
		Socket* sock;
		std::optional<HttpResponse> res; // Output

		HttpRequestTask(Scheduler* sched, const Uri& uri);

		void onTick() final;

	protected:
		[[nodiscard]] bool shouldRecycle() const noexcept;
		void cannotRecycle();

		void sendRequest();
	};
}

#endif
