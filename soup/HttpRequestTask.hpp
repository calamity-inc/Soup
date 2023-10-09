#pragma once

#include "Task.hpp"

#include <optional>

#include "base.hpp"
#include "HttpResponse.hpp"
#include "Uri.hpp"
#if !SOUP_WASM
#include "DelayedCtor.hpp"
#include "HttpRequest.hpp"
#include "netConnectTask.hpp"
#include "SharedPtr.hpp"
#endif

namespace soup
{
	class HttpRequestTask : public PromiseTask<std::optional<HttpResponse>>
	{
#if !SOUP_WASM
	public:
		enum State : uint8_t
		{
			START = 0,
			WAIT_FOR_OTHER_TASK_CONNECTING,
			CHECK_REUSABLE_SOCKET,
			WAIT_TO_REUSE,
			CONNECTING,
			AWAIT_RESPONSE,
		};

		State state = START;
		HttpRequest hr;
		DelayedCtor<netConnectTask> connector;
		SharedPtr<Socket> sock;
		time_t awaiting_response_since;

		HttpRequestTask(HttpRequest&& hr);
		HttpRequestTask(const Uri& uri);
		HttpRequestTask(std::string host, std::string path);

		void onTick() final;

	protected:
		[[nodiscard]] bool shouldRecycle() const noexcept;
		void doRecycle();
		void cannotRecycle();

		void sendRequest();
#else
	public:
		HttpRequestTask(const Uri& uri);

		void onTick() final;

		int getSchedulingDisposition() const final;
#endif
	};
}
