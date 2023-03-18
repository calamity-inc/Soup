#pragma once

#include "Task.hpp"

#include "Promise.hpp"
#include "Socket.hpp"

namespace soup
{
	struct netConnectTask : public Task
	{
		struct Info
		{
			IpAddr addr;
			uint16_t port;
		};

		Promise<Socket> promise;

		netConnectTask(const IpAddr& addr, uint16_t port)
			: promise([](Capture&& cap, PromiseBase* pb)
			{
				Info& info = cap.get<Info>();
				Socket sock;
				sock.connect(info.addr, info.port);
				pb->fulfil(std::move(sock));
			}, Info{ addr, port })
		{
		}

		[[nodiscard]] Socket& getSocket() noexcept
		{
			return promise.getResult();
		}

		void onTick() final
		{
			awaitPromiseCompletion(&promise);
		}
	};
}
