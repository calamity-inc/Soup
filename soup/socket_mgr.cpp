#include "socket_mgr.hpp"

namespace soup
{
	socket& socket_mgr::addSocket(socket&& sock) noexcept
	{
		pollfds.emplace_back(pollfd{ sock.fd, POLLIN });
		return sockets.emplace_back(std::move(sock));
	}

	bool socket_mgr::bind(uint16_t port) noexcept
	{
		socket sock6{};
		if (!sock6.bind6(port))
		{
			return false;
		}
		sock6.on_data_available = [](socket& s)
		{
			return s.on_data_available_capture.get<socket_mgr*>()->processAccept(s.accept6(), s.peer.port);
		};
		sock6.on_data_available_capture = this;
		addSocket(std::move(sock6));

#if SOUP_WINDOWS
		socket sock4{};
		if (!sock4.bind4(port))
		{
			return false;
		}
		sock4.on_data_available = [](socket& s)
		{
			return s.on_data_available_capture.get<socket_mgr*>()->processAccept(s.accept4(), s.peer.port);
		};
		sock4.on_data_available_capture = this;
		addSocket(std::move(sock4));
#endif

		return true;
	}

	bool socket_mgr::processAccept(socket&& sock, uint16_t port)
	{
		if (!sock.hasConnection())
		{
			return true;
		}
		socket& ref = addSocket(std::move(sock));
		if (on_accept)
		{
			on_accept(ref, port);
		}
		return false;
	}

	void socket_mgr::run()
	{
		while (!pollfds.empty())
		{
			// process sockets that we've closed
			for (auto i = sockets.begin(); i != sockets.end(); )
			{
				if (i->hasConnection())
				{
					++i;
				}
				else
				{
					fireDisconnect(i);
					pollfds.erase(socketToPollfd(i));
					i = sockets.erase(i);
				}
			}

#if SOUP_WINDOWS
			int pollret = WSAPoll(pollfds.data(), pollfds.size(), -1);
#else
			int pollret = poll(pollfds.data(), pollfds.size(), -1);
#endif
			if (pollret > 0)
			{
				for (auto i = pollfds.begin(); i != pollfds.end(); )
				{
					if (i->revents != 0)
					{
						auto sockets_i = pollfdToSocket(i);
						if (i->revents & ~POLLIN)
						{
							fireDisconnect(sockets_i);
							sockets.erase(sockets_i);
							i = pollfds.erase(i);
							continue;
						}
						if (!sockets_i->fireDataAvailable())
						{
							break;
						}
					}
					++i;
				}
			}
		}
	}

	void socket_mgr::fireDisconnect(std::vector<socket>::iterator sockets_i)
	{
		if (on_disconnect)
		{
			on_disconnect(*sockets_i);
		}
	}

	std::vector<pollfd>::iterator socket_mgr::socketToPollfd(std::vector<socket>::iterator sockets_i)
	{
		return pollfds.begin() + (sockets_i - sockets.begin());
	}

	std::vector<socket>::iterator socket_mgr::pollfdToSocket(std::vector<pollfd>::iterator pollfds_i)
	{
		return sockets.begin() + (pollfds_i - pollfds.begin());
	}
}
