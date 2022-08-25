#include "dnsHttpResolver.hpp"

#include "base64.hpp"
#include "HttpRequest.hpp"

namespace soup
{
	std::vector<dnsARecord> dnsHttpResolver::lookupA(const std::string& name) const
	{
		std::string path = "/dns-query?dns=";
		path.append(base64::urlEncode(getQueryA(name)));

		HttpRequest hr(std::string(server), std::move(path));
		auto hres = hr.execute();

		return parseResponseA(std::move(hres->body));
	}

	std::vector<dnsAaaaRecord> dnsHttpResolver::lookupAAAA(const std::string& name) const
	{
		throw 0;
	}

	std::vector<dnsSrvRecord> dnsHttpResolver::lookupSRV(const std::string& name) const
	{
		throw 0;
	}

	std::vector<dnsTxtRecord> dnsHttpResolver::lookupTXT(const std::string& name) const
	{
		throw 0;
	}
}
