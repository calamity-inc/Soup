#include "dnsCacheResolver.hpp"

#include "time.hpp"

#define LOGGING false

#if LOGGING
#include "format.hpp"
#include "log.hpp"
#endif

namespace soup
{
	std::vector<UniquePtr<dnsRecord>> dnsCacheResolver::lookup(dnsType qtype, const std::string& name) const
	{
		auto res = findInCache(qtype, name);
		if (res.empty())
		{
			res = underlying->lookup(qtype, name);
			addToCache(res);
		}
		return res;
	}

	struct dnsCachedResultTask : public dnsLookupTask
	{
		void onTick() final
		{
			SOUP_ASSERT_UNREACHABLE;
		}
	};

	struct dnsLookupAndCacheTask : public dnsLookupTask
	{
		const dnsCacheResolver& resolver;
		UniquePtr<dnsLookupTask> underlying;

		dnsLookupAndCacheTask(const dnsCacheResolver& resolver, UniquePtr<dnsLookupTask>&& underlying)
			: resolver(resolver), underlying(std::move(underlying))
		{
		}

		void onTick() final
		{
			if (underlying->tickUntilDone())
			{
				res = std::move(underlying->res);
				resolver.addToCache(res);
				setWorkDone();
			}
		}
	};

	UniquePtr<dnsLookupTask> dnsCacheResolver::makeLookupTask(dnsType qtype, const std::string& name) const
	{
		if (auto res = findInCache(qtype, name); !res.empty())
		{
			auto task = soup::make_unique<dnsCachedResultTask>();
			task->res = std::move(res);
			task->setWorkDone();
			return task;
		}
		return soup::make_unique<dnsLookupAndCacheTask>(*this, underlying->makeLookupTask(qtype, name));
	}

	std::vector<UniquePtr<dnsRecord>> dnsCacheResolver::findInCache(dnsType qtype, const std::string& name) const
	{
		std::vector<UniquePtr<dnsRecord>> res;
		for (auto i = cache.begin(); i != cache.end(); )
		{
			if (i->getExpiry() < time::unixSeconds())
			{
				i = cache.erase(i);				
#if LOGGING
				logWriteLine(format("[DNS Cache] {}@{} expired", dnsTypeToString(i->record->type), i->record->name));
#endif
				continue;
			}
			if (i->record->type == qtype
				&& i->record->name == name
				)
			{
				res.emplace_back(i->record->copy());
			}
			++i;
		}
#if LOGGING
		if (res.empty())
		{
			logWriteLine(format("[DNS Cache] {}@{} miss", dnsTypeToString(qtype), name));
		}
		else
		{
			logWriteLine(format("[DNS Cache] {}@{} hit", dnsTypeToString(qtype), name));
		}
#endif
		return res;
	}

	void dnsCacheResolver::addToCache(const std::vector<UniquePtr<dnsRecord>>& records) const
	{
		if (!records.empty())
		{
			const auto t = time::unixSeconds();
			for (const auto& record : records)
			{
#if LOGGING
				logWriteLine(format("[DNS Cache] {}@{} added", dnsTypeToString(record->type), record->name));
#endif
				cache.emplace_back(CachedRecord{ record->copy(), t });
			}
		}
	}
}
