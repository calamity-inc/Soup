#include "dnsRawResolver.hpp"

#include "dnsHeader.hpp"
#include "dnsNameDecompressor.hpp"
#include "dnsQType.hpp"
#include "dnsQuestion.hpp"
#include "dnsResource.hpp"
#include "string.hpp"
#include "StringWriter.hpp"

namespace soup
{
	std::string dnsRawResolver::getQueryA(const std::string& name) const
	{
		StringWriter sw(false);
			
		dnsHeader dh{};
		dh.setRecursionDesired(true);
		dh.qdcount = 1;
		dh.write(sw);

		dnsQuestion dq;
		dq.name.name = string::explode(name, '.');
		dq.qtype = DNS_A;
		dq.write(sw);

		return sw.str;
	}

	std::vector<dnsARecord> dnsRawResolver::parseResponseA(std::string&& data) const
	{
		StringReader sr(std::move(data), false);

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
			if (dr.rtype == DNS_A
				&& dr.rclass == DNS_IN
				)
			{
				res.emplace_back(dr.ttl, *reinterpret_cast<uint32_t*>(dr.rdata.data()));
			}
		}
		return res;
	}
}
