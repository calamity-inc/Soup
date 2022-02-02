#include <iostream>

#include <dns.hpp>
#include <net_socket.hpp>

int main()
{
#define HOSTNAME "www.google.com"

	soup::net_socket sock;
	std::string data;

	std::cout << "Resolving " HOSTNAME "..." << std::endl;
	soup::net_addr_ip ip;
	if (!soup::dns::lookup(HOSTNAME, ip))
	{
		std::cout << "Lookup failed." << std::endl;
		return 1;
	}
	soup::net_addr_socket addr(ip, 443);

	std::cout << "Connecting to " << addr.toString() << "..." << std::endl;
	if (!sock.connect(addr))
	{
		std::cout << "Connect failed." << std::endl;
		return 1;
	}

	std::cout << "Negotiating TLS..." << std::endl;
	if (!sock.encrypt(HOSTNAME))
	{
		std::cout << "Encrypt failed." << std::endl;
		return 1;
	}

	std::cout << "Sending request..." << std::endl;
	if (!sock.send("GET / HTTP/1.0\r\nHost: " HOSTNAME "\r\nConnection: close\r\nUser-Agent: I'm testing my socket implementation against you\r\n\r\n"))
	{
		std::cout << "Send failed." << std::endl;
		return 1;
	}

	std::cout << "Receiving response..." << std::endl;
	if (!sock.recvAll(data))
	{
		std::cout << "Receive failed." << std::endl;
		return 1;
	}

	std::cout << data << std::endl;

	return 0;
}
