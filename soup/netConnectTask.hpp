#pragma once

#include "base.hpp"
#if !SOUP_WASM

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
		netConnectTask(Scheduler& sched, const std::string& host, uint16_t port);

		netConnectTask(const IpAddr& addr, uint16_t port)
		{
			proceedToConnect(addr, port);
		}

		void onTick() final;

		[[nodiscard]] SharedPtr<Socket> onDone(Scheduler& sched);

	protected:
		void proceedToConnect(const IpAddr& addr, uint16_t port);
	};
}

#endif
