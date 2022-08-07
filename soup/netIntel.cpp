#include "netIntel.hpp"

#include <sstream>

#include "csv.hpp"
#include "deflate.hpp"
#include "string.hpp"
#include "WebResource.hpp"

namespace soup
{
	void netIntel::init()
	{
		initAs();
		initLocation();
	}

	void netIntel::initAs()
	{
		initAsList();
		initIpv4ToAs();
		initIpv6ToAs();
	}

	void netIntel::initLocation()
	{
		initIpv4ToLocation();
		initIpv6ToLocation();
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

	void netIntel::initIpv4ToLocation()
	{
		std::stringstream ipv4tolocationcsv{};
		{
			WebResource rsc("github.com", "/sapics/ip-location-db/raw/master/dbip-city/dbip-city-ipv4-num.csv.gz");
			rsc.downloadWithCaching();
			ipv4tolocationcsv << deflate::decompress(std::move(rsc.data)).decompressed;
		}
		ipv4tolocation.reserve(3000000);
		for (std::string line; std::getline(ipv4tolocationcsv, line); )
		{
			auto arr = csv::parseLine(line);
			SOUP_IF_UNLIKELY(arr.size() < 6)
			{
				continue;
			}
			ipv4tolocation.emplace(
				string::toInt<uint32_t>(arr.at(0)).value(),
				string::toInt<uint32_t>(arr.at(1)).value(),
				netIntelLocationData{
					std::move(arr.at(2)),
					std::move(arr.at(3)),
					std::move(arr.at(5))
				}
			);
		}
	}

	void netIntel::initIpv6ToLocation()
	{
		std::stringstream ipv6tolocationcsv{};
		{
			WebResource rsc("github.com", "/sapics/ip-location-db/raw/master/dbip-city/dbip-city-ipv6.csv.gz");
			rsc.downloadWithCaching();
			ipv6tolocationcsv << deflate::decompress(std::move(rsc.data)).decompressed;
		}
		ipv6tolocation.reserve(3000000);
		for (std::string line; std::getline(ipv6tolocationcsv, line); )
		{
			auto arr = csv::parseLine(line);
			SOUP_IF_UNLIKELY(arr.size() < 6)
			{
				continue;
			}
			ipv6tolocation.emplace(arr.at(0), arr.at(1), netIntelLocationData{
				std::move(arr.at(2)),
				std::move(arr.at(3)),
				std::move(arr.at(5))
			});
		}
	}
}
