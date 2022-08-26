#include "Server.hpp"

#if !SOUP_WASM

#include "ServerService.hpp"
#include "Socket.hpp"

namespace soup
{
	struct CaptureServerPort
	{
		Server* server;
		ServerService* service;

		void processAccept(Socket&& sock, uint16_t port) const
		{
			if (sock.hasConnection())
			{
				Socket& ref = server->addSocket(std::move(sock));
				if (service->on_connection_established)
				{
					service->on_connection_established(ref, *service, *server);
				}
				if (service->on_tunnel_established)
				{
					service->on_tunnel_established(ref, *service, *server);
				}
				service->srv_on_tunnel_established(ref, *service, *server);
			}
		}
	};

	struct CaptureServerPortCrypto : public CaptureServerPort
	{
		tls_server_cert_selector_t cert_selector;
		tls_server_on_client_hello_t on_client_hello;

		void processAccept(Socket&& sock, uint16_t port) const
		{
			if (sock.hasConnection())
			{
				Socket& ref = server->addSocket(std::move(sock));
				if (service->on_connection_established)
				{
					service->on_connection_established(ref, *service, *server);
				}
				ref.enableCryptoServer(cert_selector, [](Socket& s, Capture&& _cap)
				{
					CaptureServerPortCrypto& cap = *_cap.get<CaptureServerPortCrypto*>();
					if (cap.service->on_tunnel_established)
					{
						cap.service->on_tunnel_established(s, *cap.service, *cap.server);
					}
					cap.service->srv_on_tunnel_established(s, *cap.service, *cap.server);
				}, this, on_client_hello);
			}
		}
	};

	bool Server::bind(uint16_t port, ServerService* service) noexcept
	{
		Socket sock6{};
		if (!sock6.bind6(port))
		{
			return false;
		}
		setDataAvailableHandler6(sock6);
		sock6.holdup_callback.cap = CaptureServerPort{ this, service };
		addSocket(std::move(sock6));

#if SOUP_WINDOWS
		Socket sock4{};
		if (!sock4.bind4(port))
		{
			return false;
		}
		setDataAvailableHandler4(sock4);
		sock4.holdup_callback.cap = CaptureServerPort{ this, service };
		addSocket(std::move(sock4));
#endif

		return true;
	}

	bool Server::bindCrypto(uint16_t port, ServerService* service, tls_server_cert_selector_t cert_selector, tls_server_on_client_hello_t on_client_hello) noexcept
	{
		Socket sock6{};
		if (!sock6.bind6(port))
		{
			return false;
		}
		setDataAvailableHandlerCrypto6(sock6);
		sock6.holdup_callback.cap = CaptureServerPortCrypto{ { this, service }, cert_selector, on_client_hello };
		addSocket(std::move(sock6));

#if SOUP_WINDOWS
		Socket sock4{};
		if (!sock4.bind4(port))
		{
			return false;
		}
		setDataAvailableHandlerCrypto4(sock4);
		sock4.holdup_callback.cap = CaptureServerPortCrypto{ { this, service }, cert_selector, on_client_hello };
		addSocket(std::move(sock4));
#endif

		return true;
	}

	bool Server::bindUdp(uint16_t port, udp_callback_t callback) noexcept
	{
		Socket sock6{};
		if (!sock6.udpBind6(port))
		{
			return false;
		}
		setDataAvailableHandlerUdp(sock6, callback);
		addSocket(std::move(sock6));

#if SOUP_WINDOWS
		Socket sock4{};
		if (!sock4.udpBind4(port))
		{
			return false;
		}
		setDataAvailableHandlerUdp(sock4, callback);
		addSocket(std::move(sock4));
#endif

		return true;
	}

	void Server::setDataAvailableHandler6(Socket& s)
	{
		s.holdup_type = Worker::SOCKET;
		s.holdup_callback.fp = [](Worker& w, Capture&& cap)
		{
			auto& s = reinterpret_cast<Socket&>(w);
			setDataAvailableHandler6(s);
			cap.get<CaptureServerPort>().processAccept(s.accept6(), s.peer.port);
		};
	}

	void Server::setDataAvailableHandlerCrypto6(Socket& s)
	{
		s.holdup_type = Worker::SOCKET;
		s.holdup_callback.fp = [](Worker& w, Capture&& cap)
		{
			auto& s = reinterpret_cast<Socket&>(w);
			setDataAvailableHandlerCrypto6(s);
			cap.get<CaptureServerPortCrypto>().processAccept(s.accept6(), s.peer.port);
		};
	}

#if SOUP_WINDOWS
	void Server::setDataAvailableHandler4(Socket& s)
	{
		s.holdup_type = Worker::SOCKET;
		s.holdup_callback.fp = [](Worker& w, Capture&& cap)
		{
			auto& s = reinterpret_cast<Socket&>(w);
			setDataAvailableHandler4(s);
			cap.get<CaptureServerPort>().processAccept(s.accept4(), s.peer.port);
		};
	}

	void Server::setDataAvailableHandlerCrypto4(Socket& s)
	{
		s.holdup_type = Worker::SOCKET;
		s.holdup_callback.fp = [](Worker& w, Capture&& cap)
		{
			auto& s = reinterpret_cast<Socket&>(w);
			setDataAvailableHandlerCrypto4(s);
			cap.get<CaptureServerPortCrypto>().processAccept(s.accept4(), s.peer.port);
		};
	}
#endif

	void Server::setDataAvailableHandlerUdp(Socket& s, udp_callback_t callback)
	{
		s.udpRecv([](Socket& s, SocketAddr&& sender, std::string&& data, Capture&& cap)
		{
			cap.get<udp_callback_t>()(s, std::move(sender), std::move(data));
			setDataAvailableHandlerUdp(s, cap.get<udp_callback_t>());
		}, callback);
	}
}

#endif
