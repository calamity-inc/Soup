#pragma once

#include "dnsResolver.hpp"

namespace soup
{
	struct dnsRawResolver : public dnsResolver
	{
		[[nodiscard]] std::string getQueryA(const std::string& name) const;
		[[nodiscard]] std::vector<dnsARecord> parseResponseA(std::string&& data) const;
	};
}
