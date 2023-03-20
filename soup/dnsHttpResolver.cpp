#include "dnsHttpResolver.hpp"

#if !SOUP_WASM

#include <thread>

#include "base64.hpp"
#include "DelayedCtor.hpp"
#include "HttpRequest.hpp"
#include "HttpRequestTask.hpp"
#include "Scheduler.hpp"

namespace soup
{
	struct dnsLookupWrapperTask : public Task
	{
		struct State
		{
			bool done = false;
			std::vector<UniquePtr<dnsRecord>> res;
		};

		UniquePtr<dnsLookupTask> subtask;
		State* state;

		dnsLookupWrapperTask(UniquePtr<dnsLookupTask>&& subtask, State* state)
			: subtask(std::move(subtask)), state(state)
		{
		}

		void onTick() final
		{
			if (subtask->tickUntilDone())
			{
				state->res = std::move(subtask->res);
				state->done = true;
				setWorkDone();
			}
		}
	};

	std::vector<UniquePtr<dnsRecord>> dnsHttpResolver::lookup(dnsType qtype, const std::string& name) const
	{
		if (keep_alive_sched)
		{
			auto state = soup::make_unique<dnsLookupWrapperTask::State>(); // Stack allocation wouldn't work across threads.
			keep_alive_sched->add<dnsLookupWrapperTask>(makeLookupTask(*keep_alive_sched, qtype, name), state);
			do
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			} while (!state->done);
			std::vector<UniquePtr<dnsRecord>> res = std::move(state->res);
			return res;
		}
		else
		{
			std::string path = "/dns-query?dns=";
			path.append(base64::urlEncode(getQuery(qtype, name)));

			HttpRequest hr(std::string(server), std::move(path));
			auto hres = hr.execute();

			return parseResponse(std::move(hres->body));
		}
	}

	struct dnsHttpLookupTask : public dnsLookupTask
	{
		DelayedCtor<HttpRequestTask> http;

		dnsHttpLookupTask(Scheduler& sched, IpAddr&& server, dnsType qtype, const std::string& name)
		{
			std::string url = "https://";
			url.append(server.toString());
			url.append("/dns-query?dns=");
			url.append(base64::urlEncode(dnsRawResolver::getQuery(qtype, name)));

			http.construct(sched, Uri(url));
		}

		void onTick() final
		{
			if (http->tickUntilDone())
			{
				if (http->res)
				{
					res = dnsRawResolver::parseResponse(std::move(http->res->body));
				}
				setWorkDone();
			}
		}
	};

	UniquePtr<dnsLookupTask> dnsHttpResolver::makeLookupTask(Scheduler& sched, dnsType qtype, const std::string& name) const
	{
		IpAddr server;
		SOUP_ASSERT(server.fromString(this->server));
		return soup::make_unique<dnsHttpLookupTask>(sched, std::move(server), qtype, name);
	}
}

#endif
