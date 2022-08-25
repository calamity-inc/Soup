#include "dnsHttpResolver.hpp"

#include "base64.hpp"
#include "dnsHeader.hpp"
#include "dnsNameDecompressor.hpp"
#include "dnsQType.hpp"
#include "dnsQuestion.hpp"
#include "dnsResource.hpp"
#include "HttpRequest.hpp"
#include "string.hpp"
#include "StringWriter.hpp"

namespace soup
{
	std::vector<dnsARecord> dnsHttpResolver::lookupA(const std::string& name) const
	{
		StringWriter sw(false);
		{
			dnsHeader dh;
			dh.qdcount = 1;
			dh.write(sw);

			dnsQuestion dq;
			dq.name.name = string::explode(name, '.');
			dq.qtype = DNS_A;
			dq.write(sw);
		}

		std::string path = "/dns-query?dns=";
		path.append(base64::urlEncode(sw.str));

		HttpRequest hr(std::string(server), std::move(path));
		auto hres = hr.execute();

		StringReader sr(std::move(hres->body), false);

		dnsHeader dh;
		dh.read(sr);

		dnsNameDecompressor dd;

		for (uint16_t i = 0; i != dh.qdcount; ++i)
		{
			dnsQuestion dq;
			dq.read(sr);
			dd.getString(dq);
		}

		std::vector<dnsARecord> res{};
		for (uint16_t i = 0; i != dh.ancount; ++i)
		{
			dnsResource dr;
			dr.read(sr);
			/*if (string::join(dd.getString(dr), '.') == name
				&& dr.rtype == DNS_A
				&& dr.rclass == DNS_IN
				)*/
			{
				res.emplace_back(dr.ttl, *reinterpret_cast<uint32_t*>(dr.rdata.data()));
			}
		}
		return res;
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
