#include "HttpRequestTask.hpp"
#if !SOUP_WASM

#include "ReuseTag.hpp"
#include "Scheduler.hpp"

namespace soup
{
	HttpRequestTask::HttpRequestTask(Scheduler& sched, const Uri& uri)
		: SchedulerAwareTask(sched), hr(uri)
	{
		if (shouldRecycle())
		{
			hr.setKeepAlive();
			sock = sched.findReusableSocketForHost(hr.getHost());
			if (sock)
			{
				sock->ref();
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

	void HttpRequestTask::onTick()
	{
		switch (state)
		{
		case WAIT_TO_REUSE:
			// If sock could be stale here, AWAIT_RESPONSE also has a stale pointer problem, so Scheduler needs to know that it cannot clean up the Socket.
			if (!sock)
			{
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
				sock = &connector->onDone(getScheduler());
				sock->ref();
				connector.destroy();
				if (shouldRecycle())
				{
					if (getScheduler().findReusableSocketForHost(hr.getHost()))
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
				sock->unref();
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
			s.unref();
		}, this);
	}
}

#endif
