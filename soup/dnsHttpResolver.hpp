#pragma once

#include "dnsRawResolver.hpp"

namespace soup
{
	struct dnsHttpResolver : public dnsRawResolver
	{
		std::string server = "1.1.1.1";

		[[nodiscard]] std::vector<dnsARecord> lookupA(const std::string& name) const final;
		[[nodiscard]] std::vector<dnsAaaaRecord> lookupAAAA(const std::string& name) const final;
		[[nodiscard]] std::vector<dnsSrvRecord> lookupSRV(const std::string& name) const final;
		[[nodiscard]] std::vector<dnsTxtRecord> lookupTXT(const std::string& name) const final;
	};
}