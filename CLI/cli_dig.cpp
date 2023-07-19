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
			try
			{
				t = dnsTypeFromString(argv[i]);
			}
			catch (std::exception& e)
			{
				std::cout << e.what() << "\n";
			}
		}
	}

	for (const auto& rr : r->lookup(t, argv[0]))
	{
		std::cout << dnsTypeToString(rr->type) << "\t" << rr->name << "\tvalue=" << rr->toString() << ", ttl=" << rr->ttl << "\n";
	}
}
