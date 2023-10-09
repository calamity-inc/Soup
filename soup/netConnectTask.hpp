#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "Task.hpp"

#include "DelayedCtor.hpp"
#include "dnsLookupTask.hpp"
#include "SharedPtr.hpp"
#include "Socket.hpp"

namespace soup
{
	class netConnectTask : public Task
	{
	protected:
		struct BlockingConnectTask : public Task
		{
			IpAddr addr;
			uint16_t port;

			Socket sock;

			BlockingConnectTask(const IpAddr& addr, uint16_t port)
				: addr(addr), port(port)
			{
			}

			void onTick() final;
		};

		std::string host;
		UniquePtr<dnsLookupTask> lookup;
		SharedPtr<BlockingConnectTask> connect;
		uint16_t port;
		bool current_lookup_is_ipv6 = false;
		bool second_lookup = false;

	public:
		netConnectTask(const char* host, uint16_t port, bool prefer_ipv6 = false)
			: netConnectTask(std::string(host), port, prefer_ipv6)
		{
		}

		netConnectTask(const std::string& host, uint16_t port, bool prefer_ipv6 = false);

		netConnectTask(const IpAddr& addr, uint16_t port)
		{
			proceedToConnect(addr, port);
		}

		void onTick() final;

		[[nodiscard]] bool wasSuccessful() const; // Output
		[[nodiscard]] SharedPtr<Socket> getSocket(); // Output
		[[nodiscard]] SharedPtr<Socket> getSocket(Scheduler& sched); // Output

	protected:
		void proceedToConnect(const IpAddr& addr, uint16_t port);
	};
}

#endif
