#include <iostream>
#include <thread>

#include <server.hpp>

int main()
{
	soup::server srv;
	if (!srv.init(1337))
	{
		std::cout << "Init failed. Is port 1337 available?" << std::endl;
		return 1;
	}
	std::cout << "Listening on *:1337..." << std::endl;
	while (true)
	{
		soup::client client = srv.accept();
		if (!client.isValid())
		{
			continue;
		}
		auto t = std::thread([client{std::move(client)}]
		{
			auto name = client.peer.toString();

			std::cout << name << " + connected" << std::endl;
			while (true)
			{
				constexpr auto bufsize = 1024;
				std::string buf(bufsize, 0);
				int read = recv(client.fd, &buf.at(0), bufsize, 0);
				if(read <= 0) // -1 = error, 0 = closed
				{
					break;
				}
				buf.resize(read);
				std::cout << name << " > " << buf << std::endl;
			}
			std::cout << name << " - disconnected" << std::endl;
		});
		t.detach();
	}
}
