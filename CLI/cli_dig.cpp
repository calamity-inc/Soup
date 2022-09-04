#include "cli.hpp"

#include <iostream>

#include "dnsUdpResolver.hpp"
#include "dnsHttpResolver.hpp"
#include "dnsOsResolver.hpp"

using namespace soup;

void cli_dig(int argc, const char** argv)
{
	if (argc == 0)
	{
		std::cout << "Syntax: soup dig [domain] <type=A> <@<doh:>[server]>\n";
		return;
	}

	UniquePtr<dnsResolver> r = soup::make_unique<dnsOsResolver>();
	dnsType t = DNS_A;

	for (int i = 1; i != argc; ++i)
	{
		if (argv[i][0] == '@')
		{
			std::string server = (argv[i] + 1);
			if (server.substr(0, 4) == "doh:")
			{
				r = soup::make_unique<dnsHttpResolver>();
				reinterpret_cast<dnsHttpResolver*>(r.get())->server = server.substr(4);
				std::cout << "Using DoH resolver at " << reinterpret_cast<dnsHttpResolver*>(r.get())->server << "\n";
			}
			else
			{
				r = soup::make_unique<dnsUdpResolver>();
				reinterpret_cast<dnsUdpResolver*>(r.get())->server.ip = std::move(server);
				std::cout << "Using UDP resolver at " << reinterpret_cast<dnsUdpResolver*>(r.get())->server.toString() << "\n";
			}
		}
		else
		{
			if (strcmp(argv[i], "AAAA") == 0)
			{
				t = DNS_AAAA;
			}
			else if (strcmp(argv[i], "CNAME") == 0)
			{
				t = DNS_CNAME;
			}
			else if (strcmp(argv[i], "PTR") == 0)
			{
				t = DNS_PTR;
			}
			else if (strcmp(argv[i], "TXT") == 0)
			{
				t = DNS_TXT;
			}
			else if (strcmp(argv[i], "A") != 0)
			{
				std::cout << "Unsupported type: " << argv[i] << "\n";
			}
		}
	}

	for (const auto& rr : r->lookup(t, argv[0]))
	{
		std::cout << dnsTypeToString(rr->type) << "\t" << rr->name << "\tvalue=" << rr->getValueString() << ", ttl=" << rr->ttl << "\n";
	}
}
