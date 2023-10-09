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

	netConnectTask::netConnectTask(const std::string& host, uint16_t port)
	{
		if (IpAddr ip; ip.fromString(host))
		{
			proceedToConnect(ip, port);
		}
		else
		{
			lookup = netConfig::get().dns_resolver->makeLookupTask(DNS_A, host);

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
					if (lookup->result.empty())
					{
						// No DNS results, bail
						setWorkDone();
					}
					else
					{
						proceedToConnect(rand(dnsResolver::simplifyIPv6LookupResults(lookup->result)), port);
						lookup.reset();
					}
				}
				else
				{
					// IPv4 Result
					if (lookup->result.empty())
					{
						lookup = netConfig::get().dns_resolver->makeLookupTask(DNS_AAAA, host);
						ipv6_lookup = true;
					}
					else
					{
						proceedToConnect(rand(dnsResolver::simplifyIPv4LookupResults(lookup->result)), port);
						lookup.reset();
					}
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
