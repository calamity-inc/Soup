#include "truHostTask.hpp"
#if SOUP_WINDOWS || SOUP_LINUX

#include "CertStore.hpp"
#include "netAdaptor.hpp"

NAMESPACE_SOUP
{
	truHostTask::truHostTask(const IpAddr& relay, std::string id, SharedPtr<CertStore> faketls_cert)
		: relay_connector(relay, 7987), id(std::move(id)), faketls_cert(std::move(faketls_cert))
	{
		lan_service.should_accept_websocket_connection = [](Socket&, const HttpRequest&, ServerWebService&) -> bool
		{
			return true;
		};
		lan_service.on_websocket_message = [](WebSocketMessage& msg, Socket& s, ServerWebService& srv) -> void
		{
			// Clang warns about offsetof on non-standard layout which makes no sense to me so let's just reinvent the offsetof wheel...
			#define SOUP_OFFSETOF(c, m) reinterpret_cast<uintptr_t>(&reinterpret_cast<c*>(0)->m)
			const auto self = reinterpret_cast<truHostTask*>(reinterpret_cast<uintptr_t>(&srv) - SOUP_OFFSETOF(truHostTask, lan_service));
			if (msg.data == self->id && !self->out_sock)
			{
				ServerWebService::wsSendText(s, msg.data);
				self->ctrl_sock->close();
				self->out_sock = Scheduler::get()->getShared(s);
			}
		};
	}

	void truHostTask::onTick()
	{
		if (!sent_h)
		{
			if (relay_connector.tickUntilDone())
			{
				ctrl_sock = relay_connector.getSocket();
				sent_h = true;

				std::string lan_ips;
				uint16_t port = 0;
				for (const auto& ad : netAdaptor::getAll())
				{
					if (!ad.isVirtual())
					{
						if (!lan_ips.empty())
						{
							lan_ips.push_back(',');
						}
						IpAddr ip_addr(ad.ip_addr);
						if (faketls_cert)
						{
							port = lan_server.bindOptCrypto(ip_addr, port, &lan_service, faketls_cert);
						}
						else
						{
							port = lan_server.bind(ip_addr, port, &lan_service);
						}
						lan_ips.append(ip_addr.toString());
					}
				}
				if (lan_ips.empty())
				{
					lan_ips = "127.0.0.1";
				}

				ctrl_sock->wsSend("h " + id + " " + std::to_string(port) + " " + lan_ips);
				ctrlRecv();
			}
		}
		else
		{
			lan_server.tick();
			if (out_sock)
			{
				if (!relaying)
				{
					Scheduler::get()->addWorker(out_sock);
				}
				setWorkDone();
			}
		}
	}

	void truHostTask::ctrlRecv()
	{
		ctrl_sock->wsRecv([](WebSocketConnection& s, WebSocketMessage&& msg, Capture&& cap) -> void
		{
			const auto self = cap.get<truHostTask*>();
			//std::cout << "ctrl msg: " << msg.data << "\n";
			// Might want to handle "wait" message?
			if (msg.data == "relay")
			{
				self->out_sock = self->ctrl_sock;
				self->relaying = true;
				s.keepAlive();
			}
			else
			{
				self->ctrlRecv();
			}
		}, this);
	}
}
#endif
