#include "cli.hpp"

#include <iostream>

#include "base64.hpp"
#include "dnsType.hpp"
#include "netInfo.hpp"
#include "netMesh.hpp"
#include "netMeshMsgType.hpp"
#include "netMeshService.hpp"
#include "Packet.hpp"
#include "rand.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include "string.hpp"

using namespace soup;

SOUP_PACKET(LinkData)
{
	u32 ip;
	Bigint n;
	u64 passnum;

	SOUP_PACKET_IO(s)
	{
		return s.u32(ip)
			&& s.bigint_lp_u64_dyn(n)
			&& s.u64(passnum)
			;
	}
};

int cli_mesh(int argc, const char** argv)
{
	if (argc > 0)
	{
		std::string subcommand = argv[0];
		string::lower(subcommand);

		if (subcommand == "link")
		{
			auto& my_config = netMesh::getMyConfig();
			if (argc > 1)
			{
				LinkData data;
				data.fromBinary(base64::decode(argv[1]));

				IpAddr ip((native_u32_t)data.ip);

				std::cout << "Connecting to " << ip.toString4() << "..." << std::flush;
				Scheduler sched;
				auto sock = sched.addSocket();
				if (sock->connect(ip, 7106))
				{
					std::cout << " Connection established.\n";

					StringWriter to_encrypt;
					to_encrypt.u64(data.passnum);
					to_encrypt.str_lp_u64_dyn(my_config.kp.getPublic().getJwkThumbprint());

					auto encrypted = RsaPublicKey(data.n).encryptPkcs1(std::move(to_encrypt.data));

					StringWriter sw;
					{ uint8_t msg_type = MESH_MSG_LINK; sw.u8(msg_type); }
					sw.bigint_lp_u64_dyn(encrypted);
					sw.bigint_lp_u64_dyn(my_config.kp.n);
					sock->send(std::move(sw.data));

					std::cout << "Linking..." << std::flush;
					bool ok = false;
					sock->recv([](Socket&, std::string&& data, Capture&& cap)
					{
						if (data.at(0) == MESH_MSG_OK)
						{
							*cap.get<bool*>() = true;
						}
					}, &ok);
					sched.run();
					if (ok)
					{
						netMesh::addPeerLocally(data.n, data.ip);
						std::cout << " Successfully linked.\n";
						return 0;
					}
					else
					{
						std::cout << " Linking failed.\n";
					}
				}
				else
				{
					std::cout << " Failed to connect.\n";
				}
				return 1;
			}
			else
			{
				std::cout << "Discovering IP address..." << std::flush;
				auto ip = netInfo::getPublicAddressV4();
				std::cout << " Found " << ip.toString4() << "\n";

				Server serv;
				if (!serv.bind(7106, &g_mesh_service))
				{
					std::cout << "Failed to bind to TCP/7106\n";
					return 1;
				}

				g_mesh_service.link_passnum = soup::rand(0, -1);

				LinkData data;
				data.ip = ip.getV4NativeEndian();
				data.n = my_config.kp.n;
				data.passnum = g_mesh_service.link_passnum;

				std::cout << "\nReady to link; run this on the other machine:\nsoup mesh link " << base64::encode(data.toBinaryString()) << "\n\n";

				serv.run();
			}
			return 0;
		}

		if (subcommand == "list")
		{
			auto& config = netMesh::getMyConfig();
			if (config.peers.empty())
			{
				std::cout << "No peers configured with this machine.\n";
			}
			else
			{
				for (const auto& peer : config.peers)
				{
					if (peer.ip)
					{
						std::cout << "- " << IpAddr((native_u32_t)peer.ip).toString4() << "\n";
					}
					else
					{
						std::cout << "- Administrative device " << string::hex(peer.n_hash) << "\n";
					}
				}
			}
			return 0;
		}

		if (subcommand == "dns-add-record")
		{
			if (argc == 5)
			{
				IpAddr ip(argv[1]);
				std::string name = argv[2];
				auto type = (uint16_t)dnsTypeFromString(argv[3]);
				std::string data = argv[4];
				if (auto peer = netMesh::getMyConfig().findPeer(ip.getV4NativeEndian()))
				{
					std::cout << "Connecting to " << ip.toString4() << "..." << std::flush;
					Scheduler sched;
					auto sock = sched.addSocket();
					if (sock->connect(ip, 7106))
					{
						std::cout << " Connection established.\n";

						StringWriter sw;
						sw.u16(type);
						sw.str_lp_u64_dyn(name);
						sw.str_lp_u64_dyn(data);
						peer->sendAppMessage(*sock, MESH_MSG_DNS_ADD_RECORD, std::move(sw.data));

						std::cout << "Sending command..." << std::flush;
						bool ok = false;
						sock->recv([](Socket&, std::string&& data, Capture&& cap)
						{
							if (data.at(0) == MESH_MSG_OK)
							{
								*cap.get<bool*>() = true;
							}
						}, &ok);
						sched.run();
						if (ok)
						{
							std::cout << " Command executed successfully.\n";
							return 0;
						}
						else
						{
							std::cout << " Remote refused to or failed to carry out the command.\n";
						}
					}
					else
					{
						std::cout << " Failed to connect.\n";
					}
				}
				else
				{
					std::cout << "No peer found with IP " << argv[1] << "\n";
				}
				return 1;
			}
		}
	}

	std::cout << "Syntax: mesh [link <token>|list|dns-add-record [ip] [name] [type] [data]]\n";
	return 0;
}
