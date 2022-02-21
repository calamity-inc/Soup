#pragma once

#include "base.hpp"

#include <cstring> // memcpy
#include <string>
#include <vector>

#if SOUP_LINUX
#include <poll.h>
#endif

#include "client.hpp"
#include "socket.hpp"

namespace soup
{
	template <typename Client = client>
	class server
	{
	protected:
		socket sock6;
#if SOUP_WINDOWS
		socket sock4;
#endif

		std::vector<pollfd> pollfds{};
		std::vector<Client> clients{};

	public:
#if SOUP_WINDOWS
		static constexpr auto reserved_pollfds = 2;
#else
		static constexpr auto reserved_pollfds = 1;
#endif

		bool init(const uint16_t port)
		{
			if (!sock6.init(AF_INET6)
#if SOUP_WINDOWS
				|| !sock4.init(AF_INET)
#endif
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

#if SOUP_WINDOWS
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

			if (!sock6.setNonBlocking()
#if SOUP_WINDOWS
				|| !sock4.setNonBlocking()
#endif
				)
			{
				return false;
			}
#endif

			pollfds.clear();
			pollfds.reserve(reserved_pollfds);
			pollfds.emplace_back(pollfd{ sock6.fd, POLLIN });
#if SOUP_WINDOWS
			pollfds.emplace_back(pollfd{ sock4.fd, POLLIN });
#endif
			return true;
		}

		using on_client_connect_t = void(*)(Client& client);
		using on_client_disconnect_t = void(*)(Client& client);
		using on_client_data_available_t = void(*)(Client& client);

		on_client_connect_t on_client_connect = nullptr;
		on_client_disconnect_t on_client_disconnect = nullptr;
		on_client_data_available_t on_client_data_available = nullptr;

		void run()
		{
			while (true)
			{
				// process clients that we've closed
				for (auto i = clients.begin(); i != clients.end(); )
				{
					if (i->hasConnection())
					{
						++i;
					}
					else
					{
						dispatchDisconnectEvent(i);
						pollfds.erase(clientToPollfd(i));
						i = clients.erase(i);
					}
				}
				// await events
#if SOUP_WINDOWS
				int pollret = WSAPoll(pollfds.data(), pollfds.size(), -1);
#else
				int pollret = poll(pollfds.data(), pollfds.size(), -1);
#endif
				if (pollret <= 0)
				{
					continue;
				}
				// accept clients
				if (pollfds[0].revents & POLLIN)
				{
					runOnConnect(acceptNonBlocking6());
				}
#if SOUP_WINDOWS
				if (pollfds[1].revents & POLLIN)
				{
					runOnConnect(acceptNonBlocking4());
				}
#endif
				// process clients
				for (auto i = pollfds.begin() + reserved_pollfds; i != pollfds.end(); )
				{
					if (i->revents != 0)
					{
						auto clients_i = clients.begin() + ((i - pollfds.begin()) - reserved_pollfds);
						if (!(i->revents & POLLIN))
						{
							runOnDisconnect(i, clients_i);
							continue;
						}
						if (clients_i->recv_exact_remain != 0)
						{
							auto buf = clients_i->recv(clients_i->recv_exact_remain);
							if (buf.empty())
							{
								runOnDisconnect(i, clients_i);
								continue;
							}
							if (buf.size() == clients_i->recv_exact_remain)
							{
								clients_i->recv_exact_remain = 0;
								buf.insert(0, clients_i->recv_exact_buf);
								clients_i->recv_exact_buf.clear();
								clients_i->recv_exact_callback(*clients_i, std::move(buf));
							}
							else
							{
								clients_i->recv_exact_remain -= buf.size();
								clients_i->recv_exact_buf.append(buf);
							}
						}
						else
						{
							on_client_data_available(*clients_i);
						}
					}
					++i;
				}
			}
		}

	protected:
		void runOnConnect(Client&& _client)
		{
			if (_client.hasConnection())
			{
				pollfds.emplace_back(pollfd{ _client.fd, POLLIN });
				client& client = clients.emplace_back(std::move(_client));
				if (on_client_connect)
				{
					on_client_connect(client);
				}
			}
		}

		void runOnDisconnect(std::vector<pollfd>::iterator& i, typename std::vector<Client>::iterator clients_i)
		{
			dispatchDisconnectEvent(clients_i);
			clients.erase(clients_i);
			i = pollfds.erase(i);
		}

		void dispatchDisconnectEvent(typename std::vector<Client>::iterator clients_i)
		{
			if (on_client_disconnect)
			{
				on_client_disconnect(*clients_i);
			}
		}

		[[nodiscard]] std::vector<pollfd>::iterator clientToPollfd(typename std::vector<Client>::iterator clients_i)
		{
			return pollfds.begin() + (clients_i - clients.begin()) + reserved_pollfds;
		}

	public:
		Client accept()
		{
#if SOUP_WINDOWS
			int pollret = WSAPoll(pollfds.data(), reserved_pollfds, -1);
#else
			int pollret = poll(pollfds.data(), reserved_pollfds, -1);
#endif
			if (pollret <= 0)
			{
				return Client{};
			}
#if SOUP_WINDOWS
			if (pollfds[1].revents & POLLIN)
			{
				return acceptNonBlocking4();
			}
#endif
			return acceptNonBlocking6();
		}

		Client acceptNonBlocking()
		{
			Client res = acceptNonBlocking6();
#if SOUP_WINDOWS
			if (!res.isValid())
			{
				res = acceptNonBlocking4();
			}
#endif
			return res;
		}

#if SOUP_WINDOWS
		using socklen_t = int;
#endif

	protected:
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

#if SOUP_WINDOWS
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
#endif
	};
}
