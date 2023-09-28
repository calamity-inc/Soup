#include "Chatbot.hpp"

#include "FileReader.hpp"
#include "string.hpp"
#include "UniquePtr.hpp"

#include "cbCmdCoinflip.hpp"
#include "cbCmdRand.hpp"
#include "cbCmdDefine.hpp"
#include "cbCmdJoke.hpp"
#include "cbCmdArithmetic.hpp"
#include "cbCmdConvert.hpp"

#include "cbCmdDelete.hpp"

#include "cbCmdGreeting.hpp"
#include "cbCmdHowAreYou.hpp"
#include "cbCmdCapabilities.hpp"
#include "cbCmdFarewell.hpp"
#include "cbCmdOpinion.hpp"
#include "cbCmdOpinionFollowup.hpp"
#include "cbCmdThanks.hpp"

namespace soup
{
	[[nodiscard]] static std::vector<UniquePtr<cbCmd>> getAllCommandsImpl()
	{
		std::vector<UniquePtr<cbCmd>> cmds{};
		
		// Commands
		cmds.emplace_back(soup::make_unique<cbCmdCoinflip>());
		cmds.emplace_back(soup::make_unique<cbCmdRand>());
		cmds.emplace_back(soup::make_unique<cbCmdDefine>());
		cmds.emplace_back(soup::make_unique<cbCmdJoke>());
		cmds.emplace_back(soup::make_unique<cbCmdArithmetic>());
		cmds.emplace_back(soup::make_unique<cbCmdConvert>());

		// Implementables
		cmds.emplace_back(soup::make_unique<cbCmdDelete>());

		// Conversational
		cmds.emplace_back(soup::make_unique<cbCmdOpinionFollowup>()); // This is quite a specific trigger, should be above cbCmdCapabilities to avoid conflict with "what do you" trigger.
		cmds.emplace_back(soup::make_unique<cbCmdGreeting>());
		cmds.emplace_back(soup::make_unique<cbCmdHowAreYou>());
		cmds.emplace_back(soup::make_unique<cbCmdCapabilities>());
		cmds.emplace_back(soup::make_unique<cbCmdFarewell>());
		cmds.emplace_back(soup::make_unique<cbCmdOpinion>());
		cmds.emplace_back(soup::make_unique<cbCmdThanks>());

		return cmds;
	}

	const std::vector<UniquePtr<cbCmd>>& Chatbot::getAllCommands()
	{
		static auto cmds = getAllCommandsImpl();
		return cmds;
	}

	void Chatbot::intialiseResources(std::filesystem::path dir)
	{
		if (auto dictionary = dir / "dictionary.bin"; std::filesystem::is_regular_file(dictionary))
		{
			FileReader fr(dictionary);
			cbCmdDefine::dict = soup::make_shared<Dictionary>();
			cbCmdDefine::dict->read(fr);
		}

		if (auto jokebook = dir / "jokebook.bin"; std::filesystem::is_regular_file(jokebook))
		{
			FileReader fr(jokebook);
			cbCmdJoke::jb = soup::make_shared<Jokebook>();
			cbCmdJoke::jb->read(fr);
		}
	}

	cbResult Chatbot::process(const std::string& text)
	{
		cbParser p(text);
		for (const auto& cmd : getAllCommands())
		{
			if (cmd->checkTriggers(p))
			{
				return cmd->process(p);
			}
		}
		return "I'm sorry, I don't understand. :/";
	}

	std::string Chatbot::getResponse(const std::string& text)
	{
		return process(text).response;
	}
}
