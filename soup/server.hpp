#pragma once

#include <cstdint>
#include <cstring> // memcpy
#include <string>

#include "base.hpp"
#include "client.hpp"

namespace soup
{
	class server
	{
	protected:
		socket sock6;
		socket sock4;

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

			return sock6.setNonBlocking() && sock4.setNonBlocking();
		}

		client accept()
		{
			pollfd pollfds[] = {
				pollfd {
					sock6.fd,
					POLLIN,
				},
				pollfd {
					sock4.fd,
					POLLIN,
				},
			};
#if SOUP_WINDOWS
			int pollret = WSAPoll(pollfds, 2, -1);
#else
			int pollret = poll(pollfds, 2, -1);
#endif
			if (pollret <= 0)
			{
				return client{};
			}
			if (pollfds[0].revents & POLLIN)
			{
				return acceptNonBlocking6();
			}
			return acceptNonBlocking4();
		}

		client acceptNonBlocking()
		{
			client res = acceptNonBlocking6();
			if (res.isValid())
			{
				return res;
			}
			return acceptNonBlocking4();
		}

#if SOUP_WINDOWS
		using socklen_t = int;
#endif

		client acceptNonBlocking6()
		{
			client res;
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

		client acceptNonBlocking4()
		{
			client res;
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
