#include "Server.hpp"

#include "Socket.hpp"

namespace soup
{
	bool Server::bind(uint16_t port) noexcept
	{
		Socket sock6{};
		if (!sock6.bind6(port))
		{
			return false;
		}
		setDataAvailableHandler6(sock6);
		sock6.holdup_callback.cap = this;
		addSocket(std::move(sock6));

#if SOUP_WINDOWS
		Socket sock4{};
		if (!sock4.bind4(port))
		{
			return false;
		}
		setDataAvailableHandler4(sock4);
		sock4.holdup_callback.cap = this;
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
			cap.get<Server*>()->processAccept(s.accept6(), s.peer.port);
		};
	}

	void Server::setDataAvailableHandler4(Socket& s)
	{
		s.holdup_type = Worker::SOCKET;
		s.holdup_callback.fp = [](Worker& w, Capture&& cap)
		{
			auto& s = reinterpret_cast<Socket&>(w);
			setDataAvailableHandler4(s);
			cap.get<Server*>()->processAccept(s.accept4(), s.peer.port);
		};
	}

	void Server::processAccept(Socket&& sock, uint16_t port)
	{
		if (sock.hasConnection())
		{
			Socket& ref = addSocket(std::move(sock));
			if (on_accept)
			{
				on_accept(ref, port, *this);
			}
		}
	}
}
