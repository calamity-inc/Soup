#include "dhcp.hpp"

#if !SOUP_WASM

#include "DhcpMessage.hpp"
#include "rand.hpp"
#include "Scheduler.hpp"
#include "Socket.hpp"

#include <iostream>
#include "string.hpp"

NAMESPACE_SOUP
{
	using Info = dhcp::Info;

	Info dhcp::requestInfo(native_u32_t adaptor_ip_addr)
	{
		Info info;

		DhcpMessage req;
		req.type = 1; // Boot Request
		req.transaction_id = soup::rand.t<uint32_t>(0, -1);
		req.client_addr = adaptor_ip_addr;
		req.addOption(12, "Soup");
		req.addOption(53, "\x8"); // DHCP message type; Inform
		req.addOption(55, "\x06\x0f"); // Parameter request list; Domain name server, Domain name

		Scheduler sched;
		Socket& sock = *sched.addSocket();
		if (sock.initUdpBroadcast4()
			&& sock.setSourcePort4(68)
			&& sock.udpClientSend(SOUP_IPV4_NWE(255, 255, 255, 255), 67, req.toBinaryString())
			)
		{
			sock.udpRecv([](Socket&, SocketAddr&&, std::string&& data, Capture&& cap)
			{
				DhcpMessage msg;
				msg.fromBinary(data);
				if (auto dns_server = msg.findOption(0x06))
				{
					if (dns_server->size() == 4)
					{
						cap.get<Info*>()->dns_server = *(network_u32_t*)dns_server->data();
					}
				}
				if (auto domain_name = msg.findOption(0x0f))
				{
					cap.get<Info*>()->server_name = std::move(*domain_name);
				}
			}, &info);
			sched.runFor(200);
		}

		return info;
	}

	native_u32_t dhcp::requestAddress(native_u32_t server, native_u32_t netmask, const MacAddr& mac_addr)
	{
		DhcpMessage req;
		req.type = 1; // Boot Request
		req.transaction_id = soup::rand.t<uint32_t>(0, -1);
		req.mac_addr = mac_addr;
		req.addOption(12, "Soup");
		req.addOption(53, "\1"); // DHCP message type; Discover

		Socket sock;
		if (sock.initUdpBroadcast4()
			&& sock.setSourcePort4(68)
			&& sock.udpClientSend(SOUP_IPV4_NWE(255, 255, 255, 255), 67, req.toBinaryString())
			)
		{
			sock.close(); // Free up the port for requestSpecificAddress

			// Now the server will assign some IP for us to claim, but we won't get the response, so we'll just have to bruteforce it. :^)
			auto ip = (server & netmask);
			while (true)
			{
				if (requestSpecificAddress(server, ip, mac_addr))
				{
					return ip;
				}

				++ip;
				if ((ip & ~netmask) == 0)
				{
					break;
				}
			}
		}

		return 0;
	}

	bool dhcp::requestSpecificAddress(native_u32_t server, native_u32_t addr, const MacAddr& mac_addr)
	{
		DhcpMessage req;
		req.type = 1; // Boot Request
		req.transaction_id = soup::rand.t<uint32_t>(0, -1);
		req.mac_addr = mac_addr;
		req.addOption(12, "Soup");
		req.addOption(53, "\3"); // DHCP message type; Request

		// DHCP server
		req.server_addr = server;
		auto addr_ne = Endianness::toNetwork(server);
		req.addOption(54, std::string((const char*)&addr_ne, 4));

		// Requested IP address
		addr_ne = Endianness::toNetwork(addr);
		req.addOption(50, std::string((const char*)&addr_ne, 4));

		Scheduler sched;
		Socket& sock = *sched.addSocket();
		if (sock.initUdpBroadcast4()
			&& sock.setSourcePort4(68)
			&& sock.udpClientSend(SOUP_IPV4_NWE(255, 255, 255, 255), 67, req.toBinaryString())
			)
		{
			// If the router sends a NAK, it's addressed to 255.255.255.255, hence we can receive it.
			// However, if we successfully claim an address, the packet is sent to our new address.
			// So, anecdotally, we can say "got response = fail", and vice-versa.
			bool got_resp = false;
			sock.udpRecv([](Socket&, SocketAddr&&, std::string&& data, Capture&& cap)
			{
				*cap.get<bool*>() = true;
			}, &got_resp);
			sched.runFor(200);
			if (!got_resp)
			{
				return true;
			}
		}
		return false;
	}
}

#endif
