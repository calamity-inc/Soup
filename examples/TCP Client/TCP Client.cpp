#include <iostream>

#include <socket.hpp>
#include <socket_mgr.hpp>

static void recvLoop(soup::socket& s)
{
	s.recv([](soup::socket& s, std::string&& data, soup::capture&&)
	{
		std::cout << std::move(data);
		recvLoop(s);
	});
}

int main()
{
#define HOSTNAME "www.google.com"

	soup::socket sock{};
	std::cout << "Connecting to " HOSTNAME ":443..." << std::endl;
	if (!sock.connect(HOSTNAME, 443))
	{
		std::cout << "Connect failed." << std::endl;
		return 1;
	}
	soup::socket_mgr mgr{};

	std::cout << "Negotiating TLS..." << std::endl;
	mgr.addSocket(std::move(sock)).enableCryptoClient(HOSTNAME, [](soup::socket& s, soup::capture&&)
	{
		std::cout << "Sending request..." << std::endl;
		if (!s.send("GET / HTTP/1.0\r\nHost: " HOSTNAME "\r\nConnection: close\r\nUser-Agent: I'm testing my socket implementation against you\r\n\r\n"))
		{
			std::cout << "Send failed." << std::endl;
			exit(1);
		}

		std::cout << "Receiving response..." << std::endl;
		recvLoop(s);
	});
	mgr.run();

	return 0;
}
