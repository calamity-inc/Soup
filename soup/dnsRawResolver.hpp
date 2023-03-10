#pragma once

#include "dnsResolver.hpp"

namespace soup
{
	struct dnsRawResolver : public dnsResolver
	{
		[[nodiscard]] static std::string getQuery(dnsType qtype, const std::string& name);
		[[nodiscard]] static std::vector<UniquePtr<dnsRecord>> parseResponse(std::string&& data);
	};
}
