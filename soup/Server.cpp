#include "Server.hpp"

#include "Socket.hpp"

namespace soup
{
	struct CaptureServerPort
	{
		Server* server;
		Server::callback_t on_connection_established;

		void processAccept(Socket&& sock, uint16_t port) const
		{
			if (sock.hasConnection())
			{
				on_connection_established(server->addSocket(std::move(sock)), std::move(port), *server);
			}
		}
	};

	struct CaptureServerPortCrypto : public CaptureServerPort
	{
		uint16_t port;
		tls_server_cert_selector_t cert_selector;
		Server::callback_t on_tunnel_established;
		tls_server_on_client_hello_t on_client_hello;

		void processAccept(Socket&& sock, uint16_t port) const
		{
			if (sock.hasConnection())
			{
				Socket& ref = server->addSocket(std::move(sock));
				if (on_connection_established)
				{
					on_connection_established(ref, std::move(port), *server);
				}
				ref.enableCryptoServer(cert_selector, [](Socket& s, Capture&& _cap)
				{
					CaptureServerPortCrypto& cap = *_cap.get<CaptureServerPortCrypto*>();
					cap.on_tunnel_established(s, cap.port, *cap.server);
				}, this, on_client_hello);
			}
		}
	};


	bool Server::bind(uint16_t port, callback_t on_connection_established) noexcept
	{
		Socket sock6{};
		if (!sock6.bind6(port))
		{
			return false;
		}
		setDataAvailableHandler6(sock6);
		sock6.holdup_callback.cap = CaptureServerPort{ this, on_connection_established };
		addSocket(std::move(sock6));

#if SOUP_WINDOWS
		Socket sock4{};
		if (!sock4.bind4(port))
		{
			return false;
		}
		setDataAvailableHandler4(sock4);
		sock4.holdup_callback.cap = CaptureServerPort{ this, on_connection_established };
		addSocket(std::move(sock4));
#endif

		return true;
	}

	bool Server::bindCrypto(uint16_t port, tls_server_cert_selector_t cert_selector, callback_t on_tunnel_established, tls_server_on_client_hello_t on_client_hello, callback_t on_connection_established) noexcept
	{
		Socket sock6{};
		if (!sock6.bind6(port))
		{
			return false;
		}
		setDataAvailableHandlerCrypto6(sock6);
		sock6.holdup_callback.cap = CaptureServerPortCrypto{ { this, on_connection_established }, port, cert_selector, on_tunnel_established, on_client_hello };
		addSocket(std::move(sock6));

#if SOUP_WINDOWS
		Socket sock4{};
		if (!sock4.bind4(port))
		{
			return false;
		}
		setDataAvailableHandlerCrypto4(sock4);
		sock6.holdup_callback.cap = CaptureServerPortCrypto{ { this, on_connection_established }, port, cert_selector, on_tunnel_established, on_client_hello };
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
			//std::cout << "holdup_callback: " << (void*)cap.data << std::endl;
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
			//std::cout << "holdup_callback: " << (void*)cap.data << std::endl;
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
			//std::cout << "holdup_callback: " << (void*)cap.data << std::endl;
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
			//std::cout << "holdup_callback: " << (void*)cap.data << std::endl;
			cap.get<CaptureServerPortCrypto>().processAccept(s.accept4(), s.peer.port);
		};
	}
#endif
}
