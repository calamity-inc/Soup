#include "cli.hpp"

#include <iostream>

#include "dnsUdpResolver.hpp"
#include "dnsHttpResolver.hpp"
#include "netConfig.hpp"

using namespace soup;

void cli_dig(int argc, const char** argv)
{
	if (argc == 0)
	{
		std::cout << "Syntax: soup dig [domain] <type=A> <@<doh:>[server]>\n";
		return;
	}

	dnsResolver* r = netConfig::get().dns_resolver;
	UniquePtr<dnsResolver> rup;
	dnsType t = DNS_A;

	for (int i = 1; i != argc; ++i)
	{
		if (argv[i][0] == '@')
		{
			std::string server = (argv[i] + 1);
			if (server.substr(0, 4) == "doh:")
			{
				rup = soup::make_unique<dnsHttpResolver>();
				r = rup.get();
				reinterpret_cast<dnsHttpResolver*>(r)->server = server.substr(4);
				std::cout << "Using DoH resolver at " << reinterpret_cast<dnsHttpResolver*>(r)->server << "\n";
			}
			else
			{
				rup = soup::make_unique<dnsUdpResolver>();
				r = rup.get();
				reinterpret_cast<dnsUdpResolver*>(r)->server.ip.fromString(std::move(server));
				std::cout << "Using UDP resolver at " << reinterpret_cast<dnsUdpResolver*>(r)->server.toString() << "\n";
			}
		}
		else
		{
			if (!dnsTypeFromString(argv[i]).consume(t))
			{
				std::cout << "Unknown record type: " << argv[i] << "\n";
			}
		}
	}

	for (const auto& rr : r->lookup(t, argv[0]))
	{
		std::cout << dnsTypeToString(rr->type) << "\t" << rr->name << "\tvalue=" << rr->toString() << ", ttl=" << rr->ttl << "\n";
	}
}
