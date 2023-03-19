#include "HttpRequestTask.hpp"
#if !SOUP_WASM

namespace soup
{
	void HttpRequestTask::onTick()
	{
		if (connecting)
		{
			if (connector.tickUntilDone())
			{
				connecting = false;
				sock = &connector.onDone(getScheduler());
				if (hr.use_tls)
				{
					sock->enableCryptoClient(hr.getHost(), [](Socket&, Capture&& cap)
					{
						cap.get<HttpRequestTask*>()->sendRequest();
					}, this);
				}
				else
				{
					sendRequest();
				}
			}
		}
		else
		{
			if (sock->isWorkDone())
			{
				setWorkDone();
			}
		}
	}

	void HttpRequestTask::sendRequest()
	{
		hr.send(*sock);
		HttpRequest::recvResponse(*sock, [](Socket&, std::optional<HttpResponse>&& res, Capture&& cap)
		{
			cap.get<HttpRequestTask*>()->res = std::move(res);
			cap.get<HttpRequestTask*>()->setWorkDone();
		}, this);
	}
}

#endif
