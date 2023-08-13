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
		UniquePtr<dnsLookupTask> subtask;
		std::vector<UniquePtr<dnsRecord>> res;

		dnsLookupWrapperTask(UniquePtr<dnsLookupTask>&& subtask)
			: subtask(std::move(subtask))
		{
		}

		void onTick() final
		{
			if (subtask->tickUntilDone())
			{
				res = std::move(subtask->res);
				setWorkDone();
			}
		}
	};

	std::vector<UniquePtr<dnsRecord>> dnsHttpResolver::lookup(dnsType qtype, const std::string& name) const
	{
		if (keep_alive_sched)
		{
			auto task = keep_alive_sched->add<dnsLookupWrapperTask>(makeLookupTask(qtype, name));
			do
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			} while (!task->isWorkDone());
			std::vector<UniquePtr<dnsRecord>> res = std::move(task->res);
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

		dnsHttpLookupTask(IpAddr&& server, dnsType qtype, const std::string& name)
		{
			std::string url = "https://";
			url.append(server.toString());
			url.append("/dns-query?dns=");
			url.append(base64::urlEncode(dnsRawResolver::getQuery(qtype, name)));

			http.construct(Uri(url));
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

	UniquePtr<dnsLookupTask> dnsHttpResolver::makeLookupTask(dnsType qtype, const std::string& name) const
	{
		IpAddr server;
		SOUP_ASSERT(server.fromString(this->server));
		return soup::make_unique<dnsHttpLookupTask>(std::move(server), qtype, name);
	}
}

#endif
