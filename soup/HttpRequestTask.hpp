#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "Task.hpp"

#include <optional>

#include "DelayedCtor.hpp"
#include "HttpRequest.hpp"
#include "netConnectTask.hpp"
#include "SharedPtr.hpp"
#include "Uri.hpp"

namespace soup
{
	class HttpRequestTask : public PromiseTask<std::optional<HttpResponse>>
	{
	public:
		enum State : uint8_t
		{
			START = 0,
			WAIT_TO_REUSE,
			CONNECTING,
			AWAIT_RESPONSE,
		};

		State state = START;
		HttpRequest hr;
		DelayedCtor<netConnectTask> connector;
		SharedPtr<Socket> sock;

		HttpRequestTask(HttpRequest&& hr);
		HttpRequestTask(const Uri& uri);
		HttpRequestTask(std::string host, std::string path);

		void onTick() final;

	protected:
		[[nodiscard]] bool shouldRecycle() const noexcept;
		void cannotRecycle();

		void sendRequest();
	};
}

#endif
