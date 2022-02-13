#pragma once

#include "base.hpp"

#include <cstring> // memcpy
#include <string>
#include <vector>

#include "client.hpp"
#include "socket.hpp"

namespace soup
{
	template <typename Client = client>
	class server
	{
	protected:
		socket sock6;
		socket sock4;

		std::vector<pollfd> pollfds{};
		std::vector<Client> clients{};

	public:
		bool init(const uint16_t port)
		{
			if (!sock6.init(AF_INET6)
				|| !sock4.init(AF_INET)
				)
			{
				return false;
			}

			const auto port_n = htons(port);

			sockaddr_in6 addr6{};
			addr6.sin6_family = AF_INET6;
			addr6.sin6_port = port_n;
			if (bind(sock6.fd, (sockaddr*)&addr6, sizeof(addr6)) == -1)
			{
				return false;
			}
			if (listen(sock6.fd, 100) == -1)
			{
				return false;
			}

			sockaddr_in addr4{};
			addr4.sin_family = AF_INET;
			addr4.sin_port = port_n;
			if (bind(sock4.fd, (sockaddr*)&addr4, sizeof(addr4)) == -1)
			{
				return false;
			}
			if (listen(sock4.fd, 100) == -1)
			{
				return false;
			}

			if (!sock6.setNonBlocking() || !sock4.setNonBlocking())
			{
				return false;
			}

			pollfds.clear();
			pollfds.reserve(2);
			pollfds.emplace_back(pollfd{ sock6.fd, POLLIN });
			pollfds.emplace_back(pollfd{ sock4.fd, POLLIN });
			return true;
		}

		using on_client_connect_t = void(*)(Client& client);
		using on_client_disconnect_t = void(*)(Client& client);
		using on_client_data_t = void(*)(Client& client, std::string& data);

		on_client_connect_t on_client_connect = nullptr;
		on_client_disconnect_t on_client_disconnect = nullptr;
		on_client_data_t on_client_data = nullptr;

		void run()
		{
			while (true)
			{
#if SOUP_WINDOWS
				int pollret = WSAPoll(pollfds.data(), pollfds.size(), -1);
#else
				int pollret = poll(pollfds.data(), pollfds.size(), -1);
#endif
				if (pollret <= 0)
				{
					continue;
				}
				if (pollfds[0].revents & POLLIN)
				{
					auto client = acceptNonBlocking6();
					if (client.isValid())
					{
						runOnConnect(std::move(client));
					}
				}
				if (pollfds[1].revents & POLLIN)
				{
					auto client = acceptNonBlocking4();
					if (client.isValid())
					{
						runOnConnect(std::move(client));
					}
				}
				for (auto i = pollfds.begin() + 2; i != pollfds.end(); )
				{
					if (i->revents != 0)
					{
						auto clients_i = clients.begin() + ((i - pollfds.begin()) - 2);
						if (!(i->revents & POLLIN))
						{
							runOnDisconnect(i, clients_i);
							continue;
						}
						constexpr auto bufsize = 1024;
						std::string buf(bufsize, 0);
						int read = recv(i->fd, &buf.at(0), bufsize, 0);
						if (read <= 0)
						{
							runOnDisconnect(i, clients_i);
							continue;
						}
						buf.resize(read);
						on_client_data(*clients_i, buf);
					}
					++i;
				}
			}
		}

	protected:
		void runOnConnect(Client&& _client)
		{
			pollfds.emplace_back(pollfd{ _client.fd, POLLIN });
			client& client = clients.emplace_back(std::move(_client));
			if (on_client_connect)
			{
				on_client_connect(client);
			}
		}

		void runOnDisconnect(std::vector<pollfd>::iterator& i, typename std::vector<Client>::iterator clients_i)
		{
			if (on_client_disconnect)
			{
				on_client_disconnect(*clients_i);
			}
			clients.erase(clients_i);
			i = pollfds.erase(i);
		}

	public:
		Client accept()
		{
#if SOUP_WINDOWS
			int pollret = WSAPoll(pollfds.data(), 2, -1);
#else
			int pollret = poll(pollfds.data(), 2, -1);
#endif
			if (pollret <= 0)
			{
				return Client{};
			}
			if (pollfds[0].revents & POLLIN)
			{
				return acceptNonBlocking6();
			}
			return acceptNonBlocking4();
		}

		Client acceptNonBlocking()
		{
			Client res = acceptNonBlocking6();
			if (res.isValid())
			{
				return res;
			}
			return acceptNonBlocking4();
		}

#if SOUP_WINDOWS
		using socklen_t = int;
#endif

		Client acceptNonBlocking6()
		{
			Client res;
			sockaddr_in6 addr;
			socklen_t addrlen = sizeof(addr);
			res.fd = ::accept(sock6.fd, (sockaddr*)&addr, &addrlen);
			if (res.fd != -1)
			{
				memcpy(&res.peer.addr, &addr.sin6_addr, sizeof(addr.sin6_addr));
				res.peer.port = addr.sin6_port;
			}
			return res;
		}

		Client acceptNonBlocking4()
		{
			Client res;
			sockaddr_in addr;
			socklen_t addrlen = sizeof(addr);
			res.fd = ::accept(sock4.fd, (sockaddr*)&addr, &addrlen);
			if (res.fd != -1)
			{
				res.peer.addr = addr.sin_addr.s_addr;
				res.peer.port = addr.sin_port;
			}
			return res;
		}
	};
}
