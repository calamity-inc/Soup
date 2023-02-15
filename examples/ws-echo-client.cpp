#include <iostream>

#include <Scheduler.hpp>
#include <WebSocketConnection.hpp>
#include <WebSocketMessage.hpp>

using namespace soup;

int main(int argc, const char** argv)
{
	Scheduler sched;
	auto& con = static_cast<WebSocketConnection&>(sched.addSocket(soup::make_unique<WebSocketConnection>()));
	if (con.connect(std::string("ws.postman-echo.com"), 443))
	{
		std::cout << "TCP established.\n";
		con.enableCryptoClient("ws.postman-echo.com", [](Socket& sock, Capture&&)
		{
			std::cout << "TLS established.\n";
			auto& con = static_cast<WebSocketConnection&>(sock);
			con.upgrade("ws.postman-echo.com", "/raw", [](WebSocketConnection& con, Capture&&)
			{
				std::cout << "WS established.\n";
				con.wsSend("hi", true);
				con.wsRecv([](WebSocketConnection&, WebSocketMessage&& msg, Capture&&)
				{
					std::cout << "Got ws message: " << msg.data << " (text " << msg.is_text << ")\n";
				});
			});
		});
		sched.run();
	}
}
