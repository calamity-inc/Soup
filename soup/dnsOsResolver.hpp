#pragma once

#include "base.hpp"
#if !SOUP_ANDROID
#include "dnsResolver.hpp"

NAMESPACE_SOUP
{
	struct dnsOsResolver : public dnsResolver
	{
		[[nodiscard]] Optional<std::vector<UniquePtr<dnsRecord>>> lookup(dnsType qtype, const std::string& name) const final;
	};
}

#endif
