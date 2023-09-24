#include "HttpRequestTask.hpp"

#if !SOUP_WASM
#include "ReuseTag.hpp"
#include "Scheduler.hpp"
#else
#include <emscripten/fetch.h>
#endif

namespace soup
{
#if !SOUP_WASM
	HttpRequestTask::HttpRequestTask(HttpRequest&& _hr)
		: hr(std::move(_hr))
	{
	}

	HttpRequestTask::HttpRequestTask(const Uri& uri)
		: HttpRequestTask(HttpRequest(uri))
	{
	}

	HttpRequestTask::HttpRequestTask(std::string host, std::string path)
		: HttpRequestTask(HttpRequest(std::move(host), std::move(path)))
	{
	}

	void HttpRequestTask::onTick()
	{
		switch (state)
		{
		case START:
			if (shouldRecycle())
			{
				hr.setKeepAlive();
				sock = Scheduler::get()->findReusableSocketForHost(hr.getHost());
				if (sock)
				{
					if (sock->custom_data.getStructFromMap(ReuseTag).is_busy)
					{
						state = WAIT_TO_REUSE;
					}
					else
					{
						sock->custom_data.getStructFromMap(ReuseTag).is_busy = true;
						state = AWAIT_RESPONSE;
						sendRequest();
					}
					break;
				}
			}
			cannotRecycle(); // transition to CONNECTING state
			break;

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
				if (!connector->wasSuccessful())
				{
					setWorkDone();
					return;
				}
				sock = connector->getSocket();
				connector.destroy();
				if (shouldRecycle())
				{
					// Tag socket we just created for reuse if permitted by scheduler and no other reusable socket for the host exists.
					if (Scheduler::get()->dont_make_reusable_sockets
						|| Scheduler::get()->findReusableSocketForHost(hr.getHost())
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
		connector.construct(hr.getHost(), hr.port);
	}

	void HttpRequestTask::sendRequest()
	{
		hr.send(*sock);
		HttpRequest::recvResponse(*sock, [](Socket& s, std::optional<HttpResponse>&& res, Capture&& cap)
		{
			cap.get<HttpRequestTask*>()->fulfil(std::move(res));
			if (s.custom_data.isStructInMap(ReuseTag))
			{
				s.custom_data.getStructFromMap(ReuseTag).is_busy = false;
				s.keepAlive();
			}
		}, this);
	}
#else
	HttpRequestTask::HttpRequestTask(const Uri& uri)
	{
		emscripten_fetch_attr_t attr;
		emscripten_fetch_attr_init(&attr);
		strcpy(attr.requestMethod, "GET");
		attr.userData = this;
		attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
		attr.onsuccess = [](emscripten_fetch_t* fetch)
		{
			HttpResponse resp;
			resp.body = std::string(fetch->data, fetch->numBytes);
			resp.status_code = fetch->status;
			((HttpRequestTask*)fetch->userData)->fulfil(std::move(resp));
			emscripten_fetch_close(fetch);
		};
		attr.onerror = [](emscripten_fetch_t* fetch)
		{
			((HttpRequestTask*)fetch->userData)->setWorkDone();
			emscripten_fetch_close(fetch);
		};
		auto url = uri.toString();
		emscripten_fetch(&attr, url.c_str());
	}

	void HttpRequestTask::onTick()
	{
	}

	int HttpRequestTask::getSchedulingDisposition() const
	{
		return LOW_FREQUENCY;
	}
#endif
}
