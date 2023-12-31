#include "cli.hpp"

#include <string>
#include <iostream>

#include <console.hpp>
#include <HttpRequest.hpp>
#include <json.hpp>
#include <string.hpp>

using namespace soup;

static std::string assistant_msg{};

static void addMsgToState(JsonObject& state, std::string role, std::string content)
{
	auto msg = soup::make_unique<JsonObject>();
	msg->add("role", std::move(role));
	msg->add("content", std::move(content));
	
	state.at("messages").asArr().children.emplace_back(std::move(msg));
}

void cli_chatgpt(int argc, const char** argv)
{
	const char* token = argv[0];
	const char* model = argc > 1 ? argv[1] : "gpt-3.5-turbo";

	HttpRequest hr("api.openai.com", "/v1/chat/completions");
	hr.addHeader(std::string("Authorization: Bearer ") + token);
	hr.addHeader("Content-Type: application/json");

	JsonObject state;
	state.add("model", model);
	state.add("messages", soup::make_unique<JsonArray>());
	state.add("stream", true);

	console.init(false); // Enable UTF-8 support

	while (true)
	{
		std::cout << "User: ";
		std::string line;
		if (!std::getline(std::cin, line))
		{
			break;
		}
		string::trim(line);
		if (!line.empty())
		{
			addMsgToState(state, "user", std::move(line));
			std::cout << "Assistant: " << std::flush;
			hr.setPayload(state.encode());
			//std::cout << state.encode() << "\n";
			hr.executeEventStream([](std::unordered_map<std::string, std::string>&& data, const Capture&) SOUP_EXCAL
			{
				//std::cout << data.at("data") << "\n";
				if (auto update = json::decode(data.at("data")))
				{
					JsonObject& delta = update->asObj().at("choices").asArr().at(0).asObj().at("delta").asObj();
					if (delta.contains("content")
						//&& !delta.at("content").asStr().value.empty()
						)
					{
						std::cout << delta.at("content").asStr().value << std::flush;
						assistant_msg += delta.at("content").asStr().value;
					}
				}
			});
			std::cout << "\n";
			addMsgToState(state, "assistant", std::move(assistant_msg));
			assistant_msg.clear();
		}
	}
}
