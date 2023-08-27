#include "dnsResolver.hpp"

#if !SOUP_WASM

#include "DetachedScheduler.hpp"

#ifndef NDEBUG
#include "WeakRef.hpp"
#endif

namespace soup
{
	static DetachedScheduler dns_async_sched;

	struct dnsAsyncExecTask : public Task
	{
#ifndef NDEBUG
		WeakRef<dnsResolver> resolv;
#else
		const dnsResolver& resolv;
#endif
		dnsType qtype;
		std::string name;

		std::vector<UniquePtr<dnsRecord>> result;

		dnsAsyncExecTask(const dnsResolver& resolv, dnsType qtype, const std::string& name)
			:
#ifndef NDEBUG
			resolv(const_cast<dnsResolver*>(&resolv))
#else
			resolv(resolv)
#endif
			, qtype(qtype), name(name)
		{
		}

		void onTick() final
		{
#ifndef NDEBUG
			auto pResolv = resolv.getPointer();
			SOUP_ASSERT(pResolv); // Resolver has been deleted in the time the task was started. Was it stack-allocated?
			result = pResolv->lookup(qtype, name);
#else
			result = resolv.lookup(qtype, name);
#endif
			setWorkDone();
		}
	};

	struct dnsAsyncWatcherTask : public dnsLookupTask
	{
		SharedPtr<dnsAsyncExecTask> watched_task;

		dnsAsyncWatcherTask(SharedPtr<dnsAsyncExecTask>&& watched_task)
			: watched_task(std::move(watched_task))
		{
		}

		void onTick() final
		{
			if (watched_task->isWorkDone())
			{
				res = std::move(watched_task->result);
				watched_task.reset();
				setWorkDone();
			}
		}
	};

	std::vector<IpAddr> dnsResolver::lookupIPv4(const std::string& name) const
	{
		return simplifyIPv4LookupResults(lookup(DNS_A, name));
	}

	std::vector<IpAddr> dnsResolver::lookupIPv6(const std::string& name) const
	{
		return simplifyIPv6LookupResults(lookup(DNS_AAAA, name));
	}

	UniquePtr<dnsLookupTask> dnsResolver::makeLookupTask(dnsType qtype, const std::string& name) const
	{
		return soup::make_unique<dnsAsyncWatcherTask>(dns_async_sched.add<dnsAsyncExecTask>(*this, qtype, name));
	}

	std::vector<IpAddr> dnsResolver::simplifyIPv4LookupResults(const std::vector<UniquePtr<dnsRecord>>& vec)
	{
		std::vector<IpAddr> res{};
		for (const auto& r : vec)
		{
			if (r->type == DNS_A)
			{
				res.emplace_back(reinterpret_cast<dnsARecord*>(r.get())->data);
			}
		}
		return res;
	}

	std::vector<IpAddr> dnsResolver::simplifyIPv6LookupResults(const std::vector<UniquePtr<dnsRecord>>& vec)
	{
		std::vector<IpAddr> res{};
		for (const auto& r : vec)
		{
			if (r->type == DNS_AAAA)
			{
				res.emplace_back(reinterpret_cast<dnsAaaaRecord*>(r.get())->data);
			}
		}
		return res;
	}
}

#endif
