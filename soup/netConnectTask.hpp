#pragma once

#include "Task.hpp"

#include "DelayedCtor.hpp"
#include "dnsLookupTask.hpp"
#include "Promise.hpp"
#include "Socket.hpp"

namespace soup
{
	class netConnectTask : public Task
	{
	protected:
		struct ConnectInfo
		{
			IpAddr addr;
			uint16_t port;
		};

		std::string host;
		UniquePtr<dnsLookupTask> lookup;
		DelayedCtor<Promise<Socket>> connect;
		uint16_t port;
		bool ipv6_lookup = false;

	public:
		netConnectTask(Scheduler* sched, const std::string& host, uint16_t port);

		netConnectTask(const IpAddr& addr, uint16_t port)
		{
			proceedToConnect(addr, port);
		}

		[[nodiscard]] Socket getSocket() noexcept
		{
			Socket s;
			if (connect.isConstructed())
			{
				s = std::move(connect->getResult());
			}
			return s;
		}

		void onTick() final;

	protected:
		void proceedToConnect(const IpAddr& addr, uint16_t port);
	};
}
