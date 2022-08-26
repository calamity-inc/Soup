#include "dnsHttpResolver.hpp"

#include "base64.hpp"
#include "HttpRequest.hpp"

namespace soup
{
	std::vector<UniquePtr<dnsRecord>> dnsHttpResolver::lookup(dnsType qtype, const std::string& name) const
	{
		std::string path = "/dns-query?dns=";
		path.append(base64::urlEncode(getQuery(qtype, name)));

		HttpRequest hr(std::string(server), std::move(path));
		auto hres = hr.execute();

		return parseResponse(std::move(hres->body));
	}
}
