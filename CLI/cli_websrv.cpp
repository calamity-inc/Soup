#include "cli.hpp"

#include <filesystem>
#include <iostream>

#include <HttpRequest.hpp>
#include <JitModule.hpp>
#include <PhpState.hpp>
#include <Server.hpp>
#include <ServerWebService.hpp>
#include <Socket.hpp>
#include <string.hpp>
#include <time.hpp>

static std::string base_dir;

static std::unordered_map<std::string, soup::UniquePtr<soup::JitModule>> jit_modules{};

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
		soup::HttpResponse resp;
		resp.body = soup::string::fromFile(file_path);
		if (file_path.length() > 4)
		{
			if (file_path.substr(file_path.length() - 4) == ".php")
			{
				auto t = soup::time::nanos();
				soup::PhpState php;
				php.cwd = std::filesystem::path(file_path).parent_path();
				php.request_uri = req.path;
				resp.body = php.evaluate(resp.body);

				t = soup::time::nanos() - t;
				std::string timing = "PHP;dur=";
				timing.append(std::to_string(t / 1000000.0));
				resp.header_fields.emplace("Server-Timing", std::move(timing));
			}
			else if (file_path.substr(file_path.length() - 4) == ".cpp")
			{
				soup::JitModule* m;
				if (auto e = jit_modules.find(file_path); e != jit_modules.end())
				{
					m = e->second.get();
				}
				else
				{
					m = jit_modules.emplace(file_path, soup::make_unique<soup::JitModule>(file_path)).first->second.get();
				}
				
				std::string timing{};
				if (m->needsToBeCompiled())
				{
					auto t = soup::time::nanos();
					auto compiler_output = m->compile();
					t = soup::time::nanos() - t;
					timing = "Compile_JitModule;dur=";
					timing.append(std::to_string(t / 1000000.0));

					if (m->needsToBeCompiled())
					{
						resp.body = compiler_output;
					}
				}

				if (!m->needsToBeCompiled())
				{
					auto fp = (std::string(*)())m->getEntrypoint("web_module");
					if (fp == nullptr)
					{
						resp.body = "'web_module' entrypoint not found";
					}
					else
					{
						auto t = soup::time::nanos();
						resp.body = fp();
						t = soup::time::nanos() - t;
						if (!timing.empty())
						{
							timing.push_back(',');
						}
						timing.append("Run_JitModule;dur=");
						timing.append(std::to_string(t / 1000000.0));
					}
				}

				resp.header_fields.emplace("Server-Timing", std::move(timing));
			}
		}
		soup::ServerWebService::sendContent(s, std::move(resp));
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
	web_srv.on_connection_established = [](soup::Socket& s, soup::ServerService&, soup::Server&) SOUP_EXCAL
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
