#include "cli.hpp"

#include <iostream>
#include <unordered_map>

#include <FileReader.hpp>

#include "dnsServerService.hpp"
#include "netMesh.hpp"
#include "netMeshService.hpp"
#include "Server.hpp"
#include "string.hpp"
#include "StringReader.hpp"
#include "UniquePtr.hpp"

using namespace soup;

static std::unordered_map<std::string, std::vector<SharedPtr<dnsRecord>>> records{};

void cli_dnsserver(int argc, const char** argv)
{
	FileReader fr(argv[0]);
	for (std::string line; fr.getLine(line); )
	{
		auto arr = string::explode(line, '\t');
		if (arr.size() >= 3)
		{
			auto name = arr[0];
			string::lower(name);
			if (dnsType type; dnsTypeFromString(arr[1]).consume(type))
			{
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
			else
			{
				std::cout << "Unknown record type: " << arr[1] << "\n";
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

	if (argc > 1)
	{
		IpAddr addr(argv[1]);
		serv.bindUdp(addr, 53, &dns_srv);
		std::cout << "Bound to UDP/" << addr.toString() << ":53" << std::endl;
	}
	else
	{
		serv.bindUdp(53, &dns_srv);
		std::cout << "Bound to UDP/53" << std::endl;
	}

	if (netMesh::isEnabled())
	{
		if (g_mesh_service.bind(serv))
		{
			g_mesh_service.app_msg_handler = [](netMeshMsgType msg_type, std::string&& data)
			{
				if (msg_type == MESH_MSG_DNS_ADD_RECORD)
				{
					StringReader sr(std::move(data));
					if (uint16_t type; sr.u16(type))
					{
						if (auto factory = dnsRecord::getFactory((dnsType)type))
						{
							if (std::string name; sr.str_lp_u64_dyn(name))
							{
								if (auto e = records.find(name); e == records.end())
								{
									if (std::string data; sr.str_lp_u64_dyn(data))
									{
										if (auto rec = factory(std::string(name), 60, std::move(data)))
										{
											records.emplace(std::move(name), std::vector<SharedPtr<dnsRecord>>{ rec.release() });
											return true;
										}
									}
								}
							}
						}
					}
				}
				return false;
			};
			std::cout << "[Mesh] Listening for commands\n";
		}
		else
		{
			std::cout << "[Mesh] Failed to bind to TCP/7106\n";
		}
	}

	serv.run();
}
