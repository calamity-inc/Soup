#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "dnsRawResolver.hpp"

#include "dnsLookupTask.hpp"

namespace soup
{
	struct dnsHttpResolver : public dnsRawResolver
	{
		std::string server = "1.1.1.1";
		Scheduler* keep_alive_sched = nullptr;

		[[nodiscard]] std::vector<UniquePtr<dnsRecord>> lookup(dnsType qtype, const std::string& name) const final;
		[[nodiscard]] UniquePtr<dnsLookupTask> makeLookupTask(dnsType qtype, const std::string& name) const;
	};
}

#endif
