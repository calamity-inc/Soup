#include "netConnectTask.hpp"

#if !SOUP_WASM

#include "dnsSchedulableResolver.hpp"
#include "netConfig.hpp"
#include "rand.hpp"

namespace soup
{
	netConnectTask::netConnectTask(Scheduler* sched, const std::string& host, uint16_t port)
	{
		if (IpAddr ip; ip.fromString(host))
		{
			proceedToConnect(ip, port);
		}
		else
		{
			lookup = netConfig::get().schedulable_dns_resolver->makeLookupTask(sched, DNS_A, host);

			// In case we get no A records, we need enough data to start AAAA query.
			taskCapture() = sched;
			this->host = host;
			this->port = port;
		}
	}

	void netConnectTask::onTick()
	{
		if (lookup)
		{
			if (lookup->tickUntilDone())
			{
				if (ipv6_lookup)
				{
					// IPv6 Result
					if (lookup->res.empty())
					{
						// No DNS results, bail
						setWorkDone();
					}
					else
					{
						proceedToConnect(rand(dnsResolver::simplifyIPv6LookupResults(lookup->res)), port);
						lookup.reset();
					}
				}
				else
				{
					// IPv4 Result
					if (lookup->res.empty())
					{
						lookup = netConfig::get().schedulable_dns_resolver->makeLookupTask(taskCapture().get<Scheduler*>(), DNS_AAAA, host);
					}
					else
					{
						proceedToConnect(rand(dnsResolver::simplifyIPv4LookupResults(lookup->res)), port);
						lookup.reset();
					}
				}
			}
		}
		else
		{
			awaitPromiseCompletion(&*connect);
		}
	}

	void netConnectTask::proceedToConnect(const IpAddr& addr, uint16_t port)
	{
		connect.construct([](Capture&& cap, PromiseBase* pb)
		{
			ConnectInfo& info = cap.get<ConnectInfo>();
			Socket sock;
			sock.connect(info.addr, info.port);
			pb->fulfil(std::move(sock));
		}, ConnectInfo{ addr, port });
	}
}

#endif
