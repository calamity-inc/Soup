#include "netConnectTask.hpp"
#if !SOUP_WASM

#include "dnsSchedulableResolver.hpp"
#include "netConfig.hpp"
#include "rand.hpp"
#include "Scheduler.hpp"

namespace soup
{
	netConnectTask::netConnectTask(const std::string& host, uint16_t port)
	{
		// Special case for localhost since DoH won't resolve this
		if (host == "localhost")
		{
			proceedToConnect(IpAddr(SOUP_IPV4(127, 0, 0, 1)), port);
			return;
		}

		if (IpAddr ip; ip.fromString(host))
		{
			proceedToConnect(ip, port);
		}
		else
		{
			lookup = netConfig::get().schedulable_dns_resolver->makeLookupTask(DNS_A, host);

			// In case we get no A records, we need enough data to start AAAA query.
			this->host = host;

			// In order to connect after lookup, we need to remember the port.
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
						lookup = netConfig::get().schedulable_dns_resolver->makeLookupTask(DNS_AAAA, host);
						ipv6_lookup = true;
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

	SharedPtr<Socket> netConnectTask::onDone(Scheduler& sched)
	{
		SOUP_ASSERT(connect.isConstructed());
		return sched.addSocket(std::move(connect->getResult()));
	}

	void netConnectTask::proceedToConnect(const IpAddr& addr, uint16_t port)
	{
		connect.construct([](Capture&& cap, PromiseBase* pb)
		{
			ConnectInfo& info = cap.get<ConnectInfo>();
			Socket sock;
			if (!sock.connect(info.addr, info.port))
			{
				sock.close();
			}
			pb->fulfil(std::move(sock));
		}, ConnectInfo{ addr, port });
	}
}

#endif
