#pragma once

#include "dnsResolver.hpp"

namespace soup
{
	struct dnsOsResolver : public dnsResolver
	{
		// Note: Linux only returns records of matching type.
		[[nodiscard]] std::vector<UniquePtr<dnsRecord>> lookup(dnsType qtype, const std::string& name) const final;
	};
}
