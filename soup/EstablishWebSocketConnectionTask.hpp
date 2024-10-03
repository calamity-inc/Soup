#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "Task.hpp"

#include "netConnectTask.hpp"
#include "ObfusString.hpp"
#include "time.hpp"
#include "Uri.hpp"
#include "WebSocketConnection.hpp"

NAMESPACE_SOUP
{
	struct EstablishWebSocketConnectionTask : public Task
	{
		bool use_tls;
		netConnectTask connect;
		SharedPtr<WebSocketConnection> sock; // Output
		std::string host;
		std::string path;
		time_t upgrading_since = 0;

		EstablishWebSocketConnectionTask(const Uri& uri)
			: use_tls(uri.scheme != "ws"), connect(uri.host, (uri.port ? uri.port : (use_tls ? 443 : 80))), host(uri.host), path(uri.path)
		{
		}

		EstablishWebSocketConnectionTask(bool use_tls, const std::string& host, uint16_t port, const std::string& path)
			: use_tls(use_tls), connect(host, port), host(host), path(path)
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
					upgrading_since = time::millis();
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
				if (time::millisSince(upgrading_since) > 30'000)
				{
					sock->close();
				}
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

		std::string toString() const SOUP_EXCAL final
		{
			std::string str = ObfusString("EstablishWebSocketConnectionTask: ");
			if (!sock)
			{
				str.push_back('[');
				str.append(connect.toString());
				str.push_back(']');
			}
			else
			{
				str.append(ObfusString("Upgrading").str());
			}
			return str;
		}
	};
}

#endif
