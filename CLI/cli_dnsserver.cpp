#include "cli.hpp"

#include <iostream>
#include <unordered_map>

#include <dnsServerService.hpp>
#include <FileReader.hpp>
#include <json.hpp>
#include <netMesh.hpp>
#include <netMeshService.hpp>
#include <Server.hpp>
#include <string.hpp>
#include <UniquePtr.hpp>

using namespace soup;

static std::unordered_map<std::string, std::vector<SharedPtr<dnsRecord>>> records{};

static void add_record(UniquePtr<dnsRecord>&& rec)
{
	if (auto e = records.find(rec->name); e != records.end())
	{
		e->second.emplace_back(rec.release());
	}
	else
	{
		std::string name(rec->name);
		records.emplace(std::move(name), std::vector<SharedPtr<dnsRecord>>{ rec.release() });
	}
}

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
					add_record(factory(std::move(name), ttl, std::move(arr[2])));
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
		IpAddr addr;
		addr.fromString(argv[1]);
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
		if (netMesh::bind(serv))
		{
			g_mesh_service.app_msg_handlers.emplace("dns_list_records", [](Socket& s, std::string&&)
			{
				for (const auto& vec : records)
				{
					for (const auto& rec : vec.second)
					{
						JsonObject obj;
						obj.add("name", vec.first);
						obj.add("type", dnsTypeToString(rec->type));
						obj.add("value", rec->toString());
						netMeshService::reply(s, obj.encode());
					}
				}
				netMeshService::reply(s, {});
			});
			g_mesh_service.app_msg_handlers.emplace("dns_add_record", [](Socket& s, std::string&& data)
			{
				if (auto root = json::decode(data))
				{
					std::string name = root->asObj().at("name").asStr();
					if (dnsType type; dnsTypeFromString(root->asObj().at("type").asStr()).consume(type))
					{
						std::string value = root->asObj().at("value").asStr();
						if (auto factory = dnsRecord::getFactory((dnsType)type))
						{
							add_record(factory(std::move(name), 60, std::move(value)));
							netMeshService::replyAffirmative(s);
							return;
						}
					}
					netMeshService::replyNegative(s, "Invalid record type");
				}
				netMeshService::replyNegative(s, "Invalid request");
			});
			g_mesh_service.app_msg_handlers.emplace("dns_remove_record", [](Socket& s, std::string&& data)
			{
				if (auto root = json::decode(data))
				{
					std::string name = root->asObj().at("name").asStr();
					if (dnsType type; dnsTypeFromString(root->asObj().at("type").asStr()).consume(type))
					{
						std::string value = root->asObj().at("value").asStr();
						bool ok = false;
						for (auto vec = records.begin(); vec != records.end(); ++vec)
						{
							if (vec->first == name)
							{
								for (auto rec = vec->second.begin(); rec != vec->second.end(); ++rec)
								{
									if ((*rec)->type == type && (*rec)->toString() == value)
									{
										ok = true;
										vec->second.erase(rec);
										break;
									}
								}
								if (ok)
								{
									if (vec->second.empty())
									{
										records.erase(vec);
									}
									netMeshService::replyAffirmative(s);
								}
								return;
							}
						}
					}
					netMeshService::replyNegative(s, "Invalid record type");
				}
				netMeshService::replyNegative(s, "Invalid request");
			});
			std::cout << "[Mesh] Listening for commands\n";
		}
		else
		{
			std::cout << "[Mesh] Failed to bind to TCP/7106\n";
		}
	}

	serv.run();
}
