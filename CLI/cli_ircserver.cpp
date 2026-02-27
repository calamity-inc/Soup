#include "cli.hpp"
#if !SOUP_WASM

#include <iostream>

#include <IrcServer.hpp>
#include <Socket.hpp>

using namespace soup;

struct LoggingIrcServer : public soup::IrcServer
{
	void onClientConnected(Socket& s) final
	{
		std::cout << s.toString() << " has connected\n";
	}

	void onClientDisconnected(Socket& s) final
	{
		std::cout << s.toString() << " has disconnected\n";
	}

	void onClientLineReceived(Socket& s, const std::string& line) final
	{
		std::cout << s.toString() << " | " << line << "\n";
	}
};

int cli_ircserver()
{
	LoggingIrcServer serv;
	if (!serv.bind(6667, &serv.srv))
	{
		std::cout << "Failed to bind to port 6667\n";
		return 1;
	}
	std::cout << "Listening on port 6667\n";
	serv.run();
	return 0;
}

#endif
