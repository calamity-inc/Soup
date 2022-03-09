#include "server.hpp"

#include "socket.hpp"

namespace soup
{
	bool server::bind(uint16_t port) noexcept
	{
		socket sock6{};
		if (!sock6.bind6(port))
		{
			return false;
		}
		setDataAvailableHandler6(sock6);
		sock6.holdup_capture = this;
		addSocket(std::move(sock6));

#if SOUP_WINDOWS
		socket sock4{};
		if (!sock4.bind4(port))
		{
			return false;
		}
		setDataAvailableHandler4(sock4);
		sock4.holdup_capture = this;
		addSocket(std::move(sock4));
#endif

		return true;
	}

	void server::setDataAvailableHandler6(socket& s)
	{
		s.holdup_type = worker::SOCKET;
		s.holdup_callback = [](worker& w, capture&& cap)
		{
			auto& s = reinterpret_cast<socket&>(w);
			setDataAvailableHandler6(s);
			cap.get<server*>()->processAccept(s.accept6(), s.peer.port);
		};
	}

	void server::setDataAvailableHandler4(socket& s)
	{
		s.holdup_type = worker::SOCKET;
		s.holdup_callback = [](worker& w, capture&& cap)
		{
			auto& s = reinterpret_cast<socket&>(w);
			setDataAvailableHandler4(s);
			cap.get<server*>()->processAccept(s.accept4(), s.peer.port);
		};
	}

	void server::processAccept(socket&& sock, uint16_t port)
	{
		if (sock.hasConnection())
		{
			socket& ref = addSocket(std::move(sock));
			if (on_accept)
			{
				on_accept(ref, port);
			}
		}
	}
}
