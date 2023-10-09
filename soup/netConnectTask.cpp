#include "netConnectTask.hpp"
#if !SOUP_WASM

#include "DetachedScheduler.hpp"
#include "netConfig.hpp"
#include "ObfusString.hpp"
#include "rand.hpp"
#include "Scheduler.hpp"

namespace soup
{
	static DetachedScheduler async_connect_sched;

	netConnectTask::netConnectTask(const std::string& host, uint16_t port, bool prefer_ipv6)
	{
		if (IpAddr ip; ip.fromString(host))
		{
			proceedToConnect(ip, port);
		}
		else
		{
			lookup = netConfig::get().dns_resolver->makeLookupTask(prefer_ipv6 ? DNS_AAAA : DNS_A, host);
			current_lookup_is_ipv6 = prefer_ipv6;

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
				if (lookup->result.empty())
				{
					if (second_lookup)
					{
						// No DNS results, bail
						lookup.reset();
						setWorkDone();
					}
					else
					{
						current_lookup_is_ipv6 = !current_lookup_is_ipv6;
						lookup = netConfig::get().dns_resolver->makeLookupTask(current_lookup_is_ipv6 ? DNS_AAAA : DNS_A, host);
						second_lookup = true;
					}
				}
				else
				{
					std::vector<IpAddr> results{};
					if (current_lookup_is_ipv6)
					{
						results = dnsResolver::simplifyIPv6LookupResults(lookup->result);
					}
					else
					{
						results = dnsResolver::simplifyIPv4LookupResults(lookup->result);
					}
					lookup.reset();
					proceedToConnect(rand(results), port);
				}
			}
		}
		else
		{
			if (connect->isWorkDone())
			{
				setWorkDone();
			}
			else
			{
				SOUP_IF_UNLIKELY (!async_connect_sched.isActive())
				{
					SOUP_THROW(ObfusString("netConnectTask can't continue due to a race condition"));
				}
			}
		}
	}

	bool netConnectTask::wasSuccessful() const
	{
		return connect;
	}

	SharedPtr<Socket> netConnectTask::getSocket()
	{
		return getSocket(*Scheduler::get());
	}

	SharedPtr<Socket> netConnectTask::getSocket(Scheduler& sched)
	{
		SOUP_ASSERT(connect);
		auto sock = sched.addSocket(std::move(connect->sock));
		connect.reset();
		return sock;
	}

	void netConnectTask::proceedToConnect(const IpAddr& addr, uint16_t port)
	{
		connect = async_connect_sched.add<BlockingConnectTask>(addr, port);
	}

	void netConnectTask::BlockingConnectTask::onTick()
	{
		if (!sock.connect(addr, port))
		{
			sock.close();
		}
		setWorkDone();
	}
}

#endif
