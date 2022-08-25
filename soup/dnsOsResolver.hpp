#pragma once

#include "dnsResolver.hpp"

namespace soup
{
	struct dnsOsResolver : public dnsResolver
	{
		[[nodiscard]] std::vector<dnsARecord> lookupA(const std::string& name) const final;
		[[nodiscard]] std::vector<dnsAaaaRecord> lookupAAAA(const std::string& name) const final;
		[[nodiscard]] std::vector<dnsSrvRecord> lookupSRV(const std::string& name) const final;
		[[nodiscard]] std::vector<dnsTxtRecord> lookupTXT(const std::string& name) const final;
	};
}
