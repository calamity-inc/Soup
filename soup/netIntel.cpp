#include "netIntel.hpp"

#include <sstream>

#include "deflate.hpp"
#include "string.hpp"
#include "WebResource.hpp"

namespace soup
{
	void netIntel::init()
	{
		initAsList();
		initIpv4ToAs();
		initIpv6ToAs();
	}

	void netIntel::initAsList()
	{
		std::stringstream aslistcsv{};
		{
			WebResource rsc("github.com", "/Umkus/asn-ip/releases/download/latest/as.csv");
			rsc.downloadWithCaching();
			aslistcsv << std::move(rsc.data);
		}
		std::string line;
		std::getline(aslistcsv, line); // skip field names
		while (std::getline(aslistcsv, line))
		{
			auto asn_sep = line.find(',');
			SOUP_IF_UNLIKELY(asn_sep == std::string::npos)
			{
				continue;
			}
			netAs as;
			as.number = string::toInt<uint32_t>(line.substr(0, asn_sep)).value();
			++asn_sep;
			auto handle_sep = line.find(',', asn_sep);
			as.handle = line.substr(asn_sep, handle_sep - asn_sep);
			as.name = line.substr(handle_sep + 2, line.length() - handle_sep - 3);
			aslist.emplace(as.number, soup::make_unique<netAs>(std::move(as)));
		}
	}

	void netIntel::initIpv4ToAs()
	{
		std::stringstream ipv4toasntsv{};
		{
			WebResource rsc("iptoasn.com", "/data/ip2asn-v4-u32.tsv.gz");
			rsc.downloadWithCaching();
			ipv4toasntsv << deflate::decompress(std::move(rsc.data)).decompressed;
		}
		for (std::string line; std::getline(ipv4toasntsv, line); )
		{
			auto arr = string::explode(line, '\t');
			SOUP_IF_UNLIKELY(arr.size() < 5)
			{
				continue;
			}
			uint32_t asn = string::toInt<uint32_t>(arr.at(2)).value();
			if (asn == 0)
			{
				continue;
			}
			auto begin = string::toInt<uint32_t>(arr.at(0)).value();
			auto end = string::toInt<uint32_t>(arr.at(1)).value();
			netAs* as = getAsByNumber(asn);
			if (as == nullptr)
			{
				as = aslist.emplace(asn, soup::make_unique<netAs>(asn, std::move(arr.at(4)))).first->second.get();
			}
			ipv4toas.emplace(begin, end, as);
		}
	}

	void netIntel::initIpv6ToAs()
	{
		std::stringstream ipv6toasntsv{};
		{
			WebResource rsc("iptoasn.com", "/data/ip2asn-v6.tsv.gz");
			rsc.downloadWithCaching();
			ipv6toasntsv << deflate::decompress(std::move(rsc.data)).decompressed;
		}
		for (std::string line; std::getline(ipv6toasntsv, line); )
		{
			auto arr = string::explode(line, '\t');
			SOUP_IF_UNLIKELY(arr.size() < 5)
			{
				continue;
			}
			uint32_t asn = string::toInt<uint32_t>(arr.at(2)).value();
			if (asn == 0)
			{
				continue;
			}
			IpAddr begin = arr.at(0);
			IpAddr end = arr.at(1);
			netAs* as = getAsByNumber(asn);
			if (as == nullptr)
			{
				as = aslist.emplace(asn, soup::make_unique<netAs>(asn, std::move(arr.at(4)))).first->second.get();
			}
			ipv6toas.emplace(std::move(begin), std::move(end), as);
		}
	}
}
