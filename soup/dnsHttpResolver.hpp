#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "dnsRawResolver.hpp"

namespace soup
{
	struct dnsHttpResolver : public dnsRawResolver
	{
		std::string server = "1.1.1.1";

		[[nodiscard]] std::vector<UniquePtr<dnsRecord>> lookup(dnsType qtype, const std::string& name) const final;
	};
}

#endif
