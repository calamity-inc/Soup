#pragma once

#include "dnsResolver.hpp"

#include <vector>

#include "UniquePtr.hpp"

namespace soup
{
	struct dnsCacheResolver : public dnsResolver
	{
		struct CachedRecord
		{
			UniquePtr<dnsRecord> record;
			time_t obtained_at;

			[[nodiscard]] time_t getExpiry() const noexcept
			{
				return obtained_at + record->ttl;
			}
		};

		UniquePtr<dnsResolver> underlying;
		mutable std::vector<CachedRecord> cache;

		dnsCacheResolver(UniquePtr<dnsResolver>&& underlying)
			: underlying(std::move(underlying))
		{
		}

		[[nodiscard]] std::vector<UniquePtr<dnsRecord>> lookup(dnsType qtype, const std::string& name) const final;
		[[nodiscard]] UniquePtr<dnsLookupTask> makeLookupTask(dnsType qtype, const std::string& name) const final;

		[[nodiscard]] std::vector<UniquePtr<dnsRecord>> findInCache(dnsType qtype, const std::string& name) const;
		void addToCache(const std::vector<UniquePtr<dnsRecord>>& records) const;
	};
}
