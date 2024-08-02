#pragma once

#include "Task.hpp"
#if !SOUP_WASM

#include "dhcp.hpp"
#include "dnsUdpResolver.hpp"
#include "dnsHttpResolver.hpp"
#include "IpAddr.hpp"

NAMESPACE_SOUP
{
	struct netIntrospectTask : public Task
	{
		bool did_dhcp_request = false;
		dhcp::Info dhcp_info;

		dnsUdpResolver dhcp_udp_resolver;
		dnsUdpResolver udp_resolver;
		dnsHttpResolver http_resolver;

		struct LookupTaskPair
		{
			UniquePtr<dnsLookupTask> ipv4_task;
			UniquePtr<dnsLookupTask> ipv6_task;

			void init(dnsResolver& resolver)
			{
				ipv4_task = resolver.makeLookupTask(DNS_A, "testrr-1337.calamity.inc");
				ipv6_task = resolver.makeLookupTask(DNS_AAAA, "testrr-cafe.calamity.inc");
			}

			bool tickUntilDone() const
			{
				bool both_done = ipv4_task->tickUntilDone();
				both_done &= ipv6_task->tickUntilDone();
				return both_done;
			}

			void appendDiagnostic(std::string& str) const SOUP_EXCAL;
		};

		LookupTaskPair dhcp_udp_pair;
		LookupTaskPair udp_pair;
		LookupTaskPair http_pair;

		void onTick() final
		{
			if (!did_dhcp_request)
			{
				did_dhcp_request = true;
				dhcp_info = dhcp::requestInfo();

				if (dhcp_info.dns_server)
				{
					dhcp_udp_resolver.server.ip = dhcp_info.dns_server;
					dhcp_udp_pair.init(dhcp_udp_resolver);
				}

				udp_pair.init(udp_resolver);
				http_pair.init(http_resolver);
			}
			else
			{
				bool all_done = true;
				if (dhcp_info.dns_server)
				{
					all_done &= dhcp_udp_pair.tickUntilDone();
				}
				all_done &= udp_pair.tickUntilDone();
				all_done &= http_pair.tickUntilDone();
				if (all_done)
				{
					setWorkDone();
				}
			}
		}

		[[nodiscard]] std::string getDiagnosticsString() const SOUP_EXCAL
		{
			std::string str;
			if (dhcp_info.dns_server)
			{
				str += "Router";
				if (!dhcp_info.server_name.empty())
				{
					str.push_back(' ');
					str += dhcp_info.server_name;
				}
				str += " provided DNS server address ";
				str += IpAddr(dhcp_info.dns_server).toString();
				str.push_back('\n');
				dhcp_udp_pair.appendDiagnostic(str);
			}
			else
			{
				str += "Could not find a DHCP server on the network. Probably a VPN.";
			}
			str += "\nDNS over UDP with 1.1.1.1:\n";
			udp_pair.appendDiagnostic(str);
			str += "\nDNS over HTTPS (DoH) with 1.1.1.1:\n";
			http_pair.appendDiagnostic(str);
			return str;
		}
	};
}

#endif
