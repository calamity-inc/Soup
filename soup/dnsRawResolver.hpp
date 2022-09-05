#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "dnsResolver.hpp"

namespace soup
{
	struct dnsRawResolver : public dnsResolver
	{
		[[nodiscard]] std::string getQuery(dnsType qtype, const std::string& name) const;
		[[nodiscard]] std::vector<UniquePtr<dnsRecord>> parseResponse(std::string&& data) const;
	};
}

#endif
