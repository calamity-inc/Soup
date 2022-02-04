#include <iostream>

#include <dns.hpp>
#include <socket.hpp>

int main()
{
#define HOSTNAME "www.google.com"

	soup::socket sock{};
#if SOUP_PLATFORM_WINDOWS
	std::cout << "Connecting to " HOSTNAME ":443..." << std::endl;
	if (!sock.connectReliable(HOSTNAME, 443))
#else
	std::cout << "Connecting to " HOSTNAME ":80..." << std::endl;
	if (!sock.connectReliable(HOSTNAME, 80))
#endif
	{
		std::cout << "Connect failed." << std::endl;
		return 1;
	}

#if SOUP_PLATFORM_WINDOWS
	std::cout << "Negotiating TLS..." << std::endl;
	if (!sock.encrypt(HOSTNAME))
	{
		std::cout << "Encrypt failed." << std::endl;
		return 1;
	}
#endif

	std::cout << "Sending request..." << std::endl;
	if (!sock.send("GET / HTTP/1.0\r\nHost: " HOSTNAME "\r\nConnection: close\r\nUser-Agent: I'm testing my socket implementation against you\r\n\r\n"))
	{
		std::cout << "Send failed." << std::endl;
		return 1;
	}

	std::cout << "Receiving response..." << std::endl;
	std::string data;
	if (!sock.recvAll(data))
	{
		std::cout << "Receive failed." << std::endl;
		return 1;
	}
	std::cout << data << std::endl;

	return 0;
}
