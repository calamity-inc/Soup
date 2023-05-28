#include "cli.hpp"

#include <iostream>
#include <unordered_map>

#include <FileReader.hpp>

#include "dnsServerService.hpp"
#include "Server.hpp"
#include "string.hpp"
#include "UniquePtr.hpp"

using namespace soup;

static std::unordered_map<std::string, std::vector<SharedPtr<dnsRecord>>> records{};

void cli_dnsserver(const char* file)
{
	FileReader fr(file);
	for (std::string line; fr.getLine(line); )
	{
		auto arr = string::explode(line, '\t');
		if (arr.size() >= 3)
		{
			auto name = arr[0];
			auto type = dnsTypeFromString(arr[1]);
			if (auto factory = dnsRecord::getFactory(type))
			{
				uint32_t ttl = 60;
				if (arr.size() >= 4)
				{
					ttl = std::stoul(arr[3]);
				}
				auto rec = factory(std::string(name), ttl, std::move(arr[2]));
				if (auto e = records.find(name); e != records.end())
				{
					e->second.emplace_back(rec.release());
				}
				else
				{
					records.emplace(std::move(name), std::vector<SharedPtr<dnsRecord>>{ rec.release() });
				}
			}
			else
			{
				std::cout << "No factory for record type " << arr[1] << "\n";
			}
		}
		else if (!line.empty() && line.at(0) != '#')
		{
			std::cout << "Ignoring line: " << line << "\n";
		}
	}

	Server serv;

	dnsServerService dns_srv([](const std::string& name, const dnsName&, const SocketAddr&, dnsType) -> std::vector<SharedPtr<dnsRecord>>
	{
		if (auto e = records.find(name); e != records.end())
		{
			return e->second;
		}
		return {};
		/*std::vector<SharedPtr<dnsRecord>> res;
		res.emplace_back(soup::make_unique<dnsARecord>(name, 60, Endianness::toNetwork(SOUP_IPV4(1, 3, 3, 7))));
		res.emplace_back(soup::make_unique<dnsTxtRecord>(name, 60, name));
		return res;*/
	});
	serv.bindUdp(53, &dns_srv);

	serv.run();
}
