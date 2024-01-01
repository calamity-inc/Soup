#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "Task.hpp"

#include "netConnectTask.hpp"
#include "Uri.hpp"
#include "WebSocketConnection.hpp"

namespace soup
{
	struct EstablishWebSocketConnectionTask : public Task
	{
		bool use_tls;
		netConnectTask connect;
		SharedPtr<WebSocketConnection> sock; // Output
		std::string host;
		std::string path;

		EstablishWebSocketConnectionTask(const Uri& uri)
			: use_tls(uri.scheme != "ws"), connect(uri.host, (uri.port ? uri.port : (use_tls ? 443 : 80))), host(uri.host), path(uri.path)
		{
		}

		void onTick() final
		{
			if (!sock)
			{
				if (connect.tickUntilDone())
				{
					if (!connect.wasSuccessful())
					{
						setWorkDone();
						return;
					}
					sock = connect.getSocket();
					if (use_tls)
					{
						sock->enableCryptoClient(host, [](Socket& s, Capture&& cap) SOUP_EXCAL
						{
							cap.get<EstablishWebSocketConnectionTask*>()->proceedToUpgrade();
						}, this);
					}
					else
					{
						proceedToUpgrade();
					}
				}
			}
			else
			{
				if (sock->isWorkDoneOrClosed())
				{
					setWorkDone();
				}
			}
		}

		void proceedToUpgrade() SOUP_EXCAL
		{
			sock->upgrade(std::move(host), std::move(path), [](WebSocketConnection& con, Capture&& cap) noexcept
			{
				cap.get<EstablishWebSocketConnectionTask*>()->setWorkDone();
			}, this);
		}
	};
}

#endif
