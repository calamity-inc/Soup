#include "cbParser.hpp"

#include "string.hpp"

namespace soup
{
	cbParser::cbParser(const std::string& text)
		: words(string::explode(text, ' ')), command(words.end())
	{
	}

	static void prunePunctuation(std::string& str)
	{
		string::replace_all(str, ".", "");
		string::replace_all(str, "?", "");
		string::replace_all(str, "!", "");
	}

	[[nodiscard]] static std::string simplifyForTriggerMatch(std::string word)
	{
		prunePunctuation(word);
		string::lower(word);
		return word;
	}

	bool cbParser::checkTrigger(const std::string& trigger)
	{
		for (auto i = words.begin(); i != words.end(); ++i)
		{
			if (simplifyForTriggerMatch(*i) == trigger)
			{
				command = i;
				return true;
			}
		}
		return false;
	}

	std::string cbParser::getArgWord() const noexcept
	{
		// assuming hasCommand() is true
		auto i = command;
		if (++i != words.end())
		{
			std::string arg = *i;
			if ((i + 1) == words.end())
			{
				prunePunctuation(arg);
			}
			return arg;
		}
		return {};
	}
}
