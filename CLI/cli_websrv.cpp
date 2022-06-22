#include "cli.hpp"

#include <filesystem>
#include <iostream>

#include <HttpRequest.hpp>
#include <php.hpp>
#include <Server.hpp>
#include <ServerWebService.hpp>
#include <Socket.hpp>
#include <string.hpp>

static std::string base_dir;

static void handleRequest(soup::Socket& s, soup::HttpRequest&& req, soup::ServerWebService&)
{
	auto host = req.header_fields.find("Host");
	if (host == req.header_fields.end()
		|| host->second.empty()
		)
	{
		std::cout << s.peer.toString() << " > " << req.method << " " << req.path << " [400 - no host]" << std::endl;
		soup::ServerWebService::sendContent(s, "400", "400 - Missing 'Host' header");
		return;
	}

	std::string req_url = host->second;
	req_url.append(req.path);

	if (req_url.find("..") != std::string::npos)
	{
		std::cout << s.peer.toString() << " > " << req.method << " " << req.path << " [400 - path traversal]" << std::endl;
		soup::ServerWebService::sendContent(s, "400", "400 - Potential path traversal attack");
		return;
	}

	

	std::string file_path = base_dir;
	file_path.append(req_url);

	if (req.path == "/")
	{
		file_path.append("index.html");

		if (!std::filesystem::is_regular_file(file_path))
		{
			file_path = base_dir;
			file_path.append(host->second);
			file_path.append("/index.php");
		}
	}
	else
	{
		if (!std::filesystem::is_regular_file(file_path))
		{
			file_path.append(".html");

			if (!std::filesystem::is_regular_file(file_path))
			{
				file_path = base_dir;
				file_path.append(req_url);
				file_path.append(".php");
			}
		}
	}

	if (std::filesystem::is_regular_file(file_path))
	{
		std::cout << s.peer.toString() << " > " << req.method << " " << req_url << " [200]" << std::endl;
		auto contents = soup::string::fromFile(file_path);
		if (file_path.length() > 4 && file_path.substr(file_path.length() - 4) == ".php")
		{
			contents = soup::php::evaluate(contents);
		}
		soup::ServerWebService::sendContent(s, std::move(contents));
	}
	else
	{
		std::cout << s.peer.toString() << " > " << req.method << " " << req_url << " [404]" << std::endl;
		soup::ServerWebService::sendContent(s, "404", "404 - Page not found");
	}
}

int cli_websrv(const char* dir)
{
	base_dir = dir;
	base_dir.push_back('/');

	soup::Server serv{};
	serv.on_work_done = [](soup::Worker& w, soup::Scheduler&)
	{
		std::cout << reinterpret_cast<soup::Socket&>(w).peer.toString() << " - work done" << std::endl;
	};
	serv.on_connection_lost = [](soup::Socket& s, soup::Scheduler&)
	{
		std::cout << s.peer.toString() << " - connection lost" << std::endl;
	};
	serv.on_exception = [](soup::Worker& w, const std::exception& e, soup::Scheduler&)
	{
		std::cout << reinterpret_cast<soup::Socket&>(w).peer.toString() << " - exception: " << e.what() << std::endl;
	};

	soup::ServerWebService web_srv{ &handleRequest };
	web_srv.on_connection_established = [](soup::Socket& s, soup::ServerService&, soup::Server&)
	{
		std::cout << s.peer.toString() << " + connection established" << std::endl;
	};

	if (!serv.bind(80, &web_srv))
	{
		std::cout << "Failed to bind to port 80." << std::endl;
		return 1;
	}
	std::cout << "Listening on port 80." << std::endl;
	serv.run();

	return 0;
}
