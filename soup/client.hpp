#pragma once

#include "socket.hpp"

#include "addr_socket.hpp"

namespace soup
{
	struct client : public socket
	{
		addr_socket peer;
	};
}
