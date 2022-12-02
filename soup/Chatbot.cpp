#include "Chatbot.hpp"

#include "string.hpp"
#include "UniquePtr.hpp"

#include "cbCmdCoinflip.hpp"
#include "cbCmdDefine.hpp"

#include "cbCmdGreeting.hpp"

namespace soup
{
	[[nodiscard]] static std::vector<UniquePtr<cbCmd>> getAllCommandsImpl()
	{
		std::vector<UniquePtr<cbCmd>> cmds{};
		
		// Commands
		cmds.emplace_back(soup::make_unique<cbCmdCoinflip>());
		cmds.emplace_back(soup::make_unique<cbCmdDefine>());

		// Conversational
		cmds.emplace_back(soup::make_unique<cbCmdGreeting>());

		return cmds;
	}

	const std::vector<UniquePtr<cbCmd>>& Chatbot::getAllCommands()
	{
		static auto cmds = getAllCommandsImpl();
		return cmds;
	}

	std::string Chatbot::getResponse(const std::string& text)
	{
		cbParser p(text);
		for (const auto& cmd : getAllCommands())
		{
			for (const auto& triggerword : cmd->getTriggerwords())
			{
				if (p.checkTriggerword(triggerword))
				{
					return cmd->getResponse(p);
				}
			}
		}
		return "I'm sorry, I don't understand.";
	}
}
