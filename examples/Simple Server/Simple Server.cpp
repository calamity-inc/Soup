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
	srv.on_client_connect = [](soup::client& client)
	{
		std::cout << client.peer.toString()  << " + connected" << std::endl;
	};
	srv.on_client_disconnect = [](soup::client& client)
	{
		std::cout << client.peer.toString() << " - disconnected" << std::endl;
	};
	srv.on_client_data = [](soup::client& client, std::string& data)
	{
		std::cout << client.peer.toString() << " > " << data << std::endl;
	};
	srv.run();
}
