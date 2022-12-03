#include "Chatbot.hpp"

#include "string.hpp"
#include "UniquePtr.hpp"

#include "cbCmdCoinflip.hpp"
#include "cbCmdRand.hpp"
#include "cbCmdDefine.hpp"
#include "cbCmdArithmetic.hpp"

#include "cbCmdDelete.hpp"

#include "cbCmdGreeting.hpp"
#include "cbCmdFarewell.hpp"

namespace soup
{
	[[nodiscard]] static std::vector<UniquePtr<cbCmd>> getAllCommandsImpl()
	{
		std::vector<UniquePtr<cbCmd>> cmds{};
		
		// Commands
		cmds.emplace_back(soup::make_unique<cbCmdCoinflip>());
		cmds.emplace_back(soup::make_unique<cbCmdRand>());
		cmds.emplace_back(soup::make_unique<cbCmdDefine>());
		cmds.emplace_back(soup::make_unique<cbCmdArithmetic>());

		// Implementables
		cmds.emplace_back(soup::make_unique<cbCmdDelete>());

		// Conversational
		cmds.emplace_back(soup::make_unique<cbCmdGreeting>());
		cmds.emplace_back(soup::make_unique<cbCmdFarewell>());

		return cmds;
	}

	const std::vector<UniquePtr<cbCmd>>& Chatbot::getAllCommands()
	{
		static auto cmds = getAllCommandsImpl();
		return cmds;
	}

	cbResult Chatbot::process(const std::string& text)
	{
		cbParser p(text);
		for (const auto& cmd : getAllCommands())
		{
			for (const auto& trigger : cmd->getTriggers())
			{
				if (p.checkTrigger(trigger))
				{
					return cmd->process(p);
				}
			}
		}
		return "I'm sorry, I don't understand. :/";
	}

	std::string Chatbot::getResponse(const std::string& text)
	{
		return process(text).response;
	}
}
