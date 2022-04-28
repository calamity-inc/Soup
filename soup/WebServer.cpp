#include "WebServer.hpp"

#include "Socket.hpp"

namespace soup
{
	WebServer::WebServer(handle_request_t handle_request)
		: Server(), handle_request(handle_request)
	{
		Server::on_accept = [](soup::Socket& s, uint16_t port, Server& _srv)
		{
			WebServer& srv = reinterpret_cast<WebServer&>(_srv);

			if (srv.log_func)
			{
				std::string msg = s.peer.toString();
				msg.append(" + connected at port ");
				msg.append(std::to_string(port));
				srv.log_func(std::move(msg), srv);
			}

			if (srv.secure_ports.contains(port))
			{
				WebServer* pSrv = &srv;
				s.enableCryptoServer(srv.cert_selector, [](Socket& s, Capture&& cap)
				{
					cap.get<WebServer*>()->httpRecv(s);
				}, pSrv, srv.on_client_hello);
			}
			else
			{
				srv.httpRecv(s);
			}
		};
	}

	bool WebServer::bindSecure(uint16_t port)
	{
		secure_ports.emplace(port);
		return bind(port);
	}

	void WebServer::run()
	{
		if (log_func)
		{
			on_work_done = [](Worker& w, Scheduler& sched)
			{
				std::string msg = reinterpret_cast<Socket&>(w).peer.toString();
				msg.append(" - work done");
				WebServer& srv = reinterpret_cast<WebServer&>(sched);
				srv.log_func(std::move(msg), srv);
			};
			on_connection_lost = [](Socket& s, Scheduler& sched)
			{
				std::string msg = s.peer.toString();
				msg.append(" - connection lost");
				WebServer& srv = reinterpret_cast<WebServer&>(sched);
				srv.log_func(std::move(msg), srv);
			};
			on_exception = [](Worker& w, const std::exception& e, Scheduler& sched)
			{
				std::string msg = reinterpret_cast<Socket&>(w).peer.toString();
				msg.append(" - exception: ");
				msg.append(e.what());
				WebServer& srv = reinterpret_cast<WebServer&>(sched);
				srv.log_func(std::move(msg), srv);
			};
		}

		return Server::run();
	}

	void WebServer::sendHtml(Socket& s, std::string body)
	{
		auto len = body.size();
		body.insert(0, "\r\n\r\n");
		body.insert(0, std::to_string(len));
		body.insert(0, "HTTP/1.0 200\r\nServer: Soup\r\nCache-Control: private\r\nContent-Type: text/html\r\nContent-Length: ");
		s.send(body);
		s.close();
	}

	void WebServer::sendRedirect(Socket& s, const std::string& location)
	{
		std::string cont = "HTTP/1.0 302\r\nServer: Soup\r\nLocation: ";
		cont.append(location);
		cont.append("\r\nContent-Length: 0\r\n\r\n");
		s.send(std::move(cont));
		s.close();
	}

	void WebServer::httpRecv(Socket& s)
	{
		s.recv([](Socket& s, std::string&& data, Capture&& cap)
		{
			auto i = data.find(' ');
			if (i == std::string::npos)
			{
				s.send("HTTP/1.0 400\r\n\r\n");
				s.close();
				return;
			}
			data.erase(0, i + 1);
			i = data.find(' ');
			if (i == std::string::npos)
			{
				s.send("HTTP/1.0 400\r\n\r\n");
				s.close();
				return;
			}
			data.erase(i);

			WebServer& srv = *cap.get<WebServer*>();

			if (srv.log_func)
			{
				std::string msg = s.peer.toString();
				msg.append(" > ");
				msg.append(data);
				srv.log_func(std::move(msg), srv);
			}

			srv.handle_request(s, std::move(data));
		}, this);
	}
}
