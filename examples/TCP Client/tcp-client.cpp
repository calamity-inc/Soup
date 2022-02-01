#include <iostream>

#include <net_socket.hpp>

int main()
{
	soup::net_addr_socket addr("::1", 1337);
	std::cout << "Connecting to " << addr.toString() << "..." << std::endl;

	soup::net_socket sock;
	if (!sock.init(addr))
	{
		std::cout << "Connection failed." << std::endl;
		return 1;
	}

	send(sock.fd, "Bla", 4, 0);

	return 0;
}
