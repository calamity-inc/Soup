#include "dnsCacheResolver.hpp"

#include "time.hpp"

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
		return res;
	}

	void dnsCacheResolver::addToCache(const std::vector<UniquePtr<dnsRecord>>& records) const
	{
		if (!records.empty())
		{
			const auto t = time::unixSeconds();
			for (const auto& record : records)
			{
				cache.emplace_back(CachedRecord{ record->copy(), t });
			}
		}
	}
}
