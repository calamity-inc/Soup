#include "HttpRequestTask.hpp"
#if !SOUP_WASM

#include "ReuseTag.hpp"
#include "Scheduler.hpp"

namespace soup
{
	HttpRequestTask::HttpRequestTask(Scheduler& sched, HttpRequest&& _hr)
		: SchedulerAwareTask(sched), hr(std::move(_hr))
	{
		if (shouldRecycle())
		{
			hr.setKeepAlive();
			sock = sched.findReusableSocketForHost(hr.getHost());
			if (sock)
			{
				if (sock->custom_data.getStructFromMap(ReuseTag).is_busy)
				{
					state = WAIT_TO_REUSE;
				}
				else
				{
					state = AWAIT_RESPONSE;
					sendRequest();
				}
				return;
			}
		}
		cannotRecycle();
	}

	HttpRequestTask::HttpRequestTask(Scheduler& sched, const Uri& uri)
		: HttpRequestTask(sched, HttpRequest(uri))
	{
	}

	void HttpRequestTask::onTick()
	{
		switch (state)
		{
		case WAIT_TO_REUSE:
			if (sock->isWorkDoneOrClosed())
			{
				sock.reset();
				cannotRecycle();
			}
			else if (!sock->custom_data.getStructFromMap(ReuseTag).is_busy)
			{
				sock->custom_data.getStructFromMap(ReuseTag).is_busy = true;
				state = AWAIT_RESPONSE;
				sendRequest();
			}
			break;

		case CONNECTING:
			if (connector->tickUntilDone())
			{
				sock = connector->onDone(getScheduler());
				connector.destroy();
				if (shouldRecycle())
				{
					if (Scheduler::get()->dont_make_reusable_sockets
						|| getScheduler().findReusableSocketForHost(hr.getHost())
						)
					{
						hr.setClose();
					}
					else
					{
						if (!sock->custom_data.isStructInMap(ReuseTag))
						{
							sock->custom_data.getStructFromMap(ReuseTag).host = hr.getHost();
						}
					}
				}
				state = AWAIT_RESPONSE;
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
			break;

		case AWAIT_RESPONSE:
			if (sock->isWorkDoneOrClosed())
			{
				sock.reset();
				setWorkDone();
			}
			break;
		}
	}

	bool HttpRequestTask::shouldRecycle() const noexcept
	{
		return hr.use_tls && hr.port == 443;
	}

	void HttpRequestTask::cannotRecycle()
	{
		state = CONNECTING;
		connector.construct(getScheduler(), hr.getHost(), hr.port);
	}

	void HttpRequestTask::sendRequest()
	{
		hr.send(*sock);
		HttpRequest::recvResponse(*sock, [](Socket& s, std::optional<HttpResponse>&& res, Capture&& cap)
		{
			cap.get<HttpRequestTask*>()->res = std::move(res);
			cap.get<HttpRequestTask*>()->setWorkDone();
			if (s.custom_data.isStructInMap(ReuseTag))
			{
				s.custom_data.getStructFromMap(ReuseTag).is_busy = false;
				s.keepAlive();
			}
		}, this);
	}
}

#endif
