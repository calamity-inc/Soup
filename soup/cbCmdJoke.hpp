#pragma once

#include "cbCmd.hpp"

#include "Jokebook.hpp"
#include "SharedPtr.hpp"

namespace soup
{
	struct cbCmdJoke : public cbCmd
	{
		inline static SharedPtr<Jokebook> jb;
		/*** To initialise this, use Chatbot::intialiseResources or:
		* FileReader fr("Soup/resources/jokebook.bin");
		* cbCmdJoke::jb = soup::make_shared<Jokebook>();
		* cbCmdJoke::jb->read(fr);
		*/

		[[nodiscard]] std::vector<std::string> getTriggers() const noexcept final
		{
			return { "joke", "jokes" };
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			if (!jb)
			{
				return "I'd love to tell you jokes, but soup::cbCmdJoke::jb is uninitialised, so I have no data. :|";
			}
			auto type = p.getArgModifier();
			if (type.empty())
			{
				return jb->getRandomJoke().toString();
			}
			if (!jb->hasType(type))
			{
				std::string msg = "I'm sorry, I don't know any ";
				msg.append(type);
				msg.append(" jokes.");
				return cbResult(std::move(msg));
			}
			return jb->getRandomJokeByType(type).toString();
		}
	};
}
