#include "HttpRequestTask.hpp"

#if !SOUP_WASM
#include "format.hpp"
#include "log.hpp"
#include "ObfusString.hpp"
#include "ReuseTag.hpp"
#include "Scheduler.hpp"
#include "time.hpp"
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
			if (!Scheduler::get()->dont_make_reusable_sockets)
			{
				hr.setKeepAlive();
				sock = Scheduler::get()->findReusableSocket(hr.getHost(), hr.port, hr.use_tls);
				if (sock)
				{
					doRecycle();
					break;
				}
				if (Scheduler::get()->pending_reusable_sockets.find(hr.getHost()) != Scheduler::get()->pending_reusable_sockets.end())
				{
					state = WAIT_FOR_OTHER_TASK_CONNECTING;
					break;
				}
			}
			cannotRecycle(); // transition to CONNECTING state
			break;

		case WAIT_FOR_OTHER_TASK_CONNECTING:
			if (Scheduler::get()->pending_reusable_sockets.find(hr.getHost()) == Scheduler::get()->pending_reusable_sockets.end())
			{
				// Need to give another tick for Socket to go from being a "pending worker" to being a "worker."
				state = CHECK_REUSABLE_SOCKET;
			}
			break;

		case CHECK_REUSABLE_SOCKET:
			sock = Scheduler::get()->findReusableSocket(hr.getHost(), hr.port, hr.use_tls);
			if (sock)
			{
				doRecycle();
			}
			else
			{
				// We were promised a reusable socket... attempting to connect will likely result in failure, but we'll try anyway.
				cannotRecycle();
			}
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
				awaiting_response_since = time::unixSeconds();
				sendRequest();
			}
			break;

		case CONNECTING:
			if (connector->tickUntilDone())
			{
				Scheduler::get()->pending_reusable_sockets.erase(hr.getHost());
				if (!connector->wasSuccessful())
				{
					setWorkDone();
					return;
				}
				sock = connector->getSocket();
				connector.destroy();
				if (!Scheduler::get()->dont_make_reusable_sockets)
				{
					// Tag socket we just created for reuse.
					SOUP_IF_UNLIKELY (Scheduler::get()->findReusableSocket(hr.getHost(), hr.port, hr.use_tls))
					{
						logWriteLine(soup::format(ObfusString("UNEXPECTED: HttpRequestTask failed to reuse socket for {}"), hr.getHost()));
						hr.setClose();
					}
					else
					{
						sock->custom_data.getStructFromMap(ReuseTag).init(hr.getHost(), hr.port, hr.use_tls);
					}
				}
				state = AWAIT_RESPONSE;
				awaiting_response_since = time::unixSeconds();
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
			if (sock->isWorkDoneOrClosed()
				|| time::unixSecondsSince(awaiting_response_since) > 10
				)
			{
				sock->close();
				sock.reset();
				setWorkDone();
			}
			break;
		}
	}

	void HttpRequestTask::doRecycle()
	{
		if (sock->custom_data.getStructFromMap(ReuseTag).is_busy)
		{
			state = WAIT_TO_REUSE;
		}
		else
		{
			sock->custom_data.getStructFromMap(ReuseTag).is_busy = true;
			state = AWAIT_RESPONSE;
			awaiting_response_since = time::unixSeconds();
			sendRequest();
		}
	}

	void HttpRequestTask::cannotRecycle()
	{
		state = CONNECTING;
		connector.construct(hr.getHost(), hr.port);
		Scheduler::get()->pending_reusable_sockets.emplace(hr.getHost());
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
