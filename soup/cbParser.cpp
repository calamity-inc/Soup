#include "cbParser.hpp"

#include "string.hpp"

namespace soup
{
	cbParser::cbParser(const std::string& text)
		: words(string::explode(text, ' ')), command(words.end())
	{
	}

	bool cbParser::checkTriggerword(const std::string& triggerword)
	{
		for (auto i = words.begin(); i != words.end(); ++i)
		{
			if (string::equalsIgnoreCase(*i, triggerword))
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
			return *i;
		}
		return {};
	}
}
