#pragma once

#include "socket.hpp"

#include "addr_socket.hpp"

namespace soup
{
	struct client : public socket
	{
		addr_socket peer;

		using recv_exact_callback_t = void(*)(client& _this, std::string&& data);

		unsigned int recv_exact_remain = 0;
		std::string recv_exact_buf{};
		recv_exact_callback_t recv_exact_callback;

		void recvExact(unsigned int bytes, recv_exact_callback_t callback)
		{
			auto buf = recv(bytes);
			if (buf.size() == bytes)
			{
				callback(*this, std::move(buf));
			}
			else
			{
				recv_exact_remain = (bytes - buf.size());
				recv_exact_buf = std::move(buf);
				recv_exact_callback = callback;
			}
		};
	};
}
