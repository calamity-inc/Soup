#pragma once

#include "cbCmd.hpp"

#include "Jokebook.hpp"
#include "SharedPtr.hpp"

NAMESPACE_SOUP
{
	struct cbCmdJoke : public cbCmd
	{
		inline static SharedPtr<Jokebook> jb;
		/*** To initialise this, use Chatbot::intialiseResources or:
		* FileReader fr("Soup/resources/jokebook.bin");
		* cbCmdJoke::jb = soup::make_shared<Jokebook>();
		* cbCmdJoke::jb->read(fr);
		*/

		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"(\b(?:(?'type'programming) )?jokes?\b)");
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			if (!jb)
			{
				return "I'd love to tell you jokes, but soup::cbCmdJoke::jb is uninitialised, so I have no data. :|";
			}
			if (auto type = m.findGroupByName("type"))
			{
				return jb->getRandomJokeByType(type->toString()).toString();
			}
			return jb->getRandomJoke().toString();
		}
	};
}
