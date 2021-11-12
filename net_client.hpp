#pragma once

#include "net_socket.hpp"

#include "net_addr_socket.hpp"

namespace soup
{
	struct net_client : public net_socket
	{
		net_addr_socket peer;
	};
}
