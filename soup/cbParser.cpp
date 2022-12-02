#include "cbParser.hpp"

#include "string.hpp"

namespace soup
{
	cbParser::cbParser(const std::string& text)
		: words(string::explode(text, ' ')), command_begin(words.end())
	{
	}

	static void prunePunctuation(std::string& str)
	{
		string::replace_all(str, ".", "");
		string::replace_all(str, "?", "");
		string::replace_all(str, "!", "");
	}

	[[nodiscard]] static bool isNumericIgnorePunctuation(const std::string& str)
	{
		for (const auto& c : str)
		{
			if (!string::isNumberChar(c)
				&& c != '.'
				&& c != '?'
				&& c != '!'
				)
			{
				return false;
			}
		}
		return true;
	}

	[[nodiscard]] static std::string simplifyForTriggerMatch(std::string word)
	{
		prunePunctuation(word);
		string::lower(word);
		return word;
	}

	bool cbParser::checkTrigger(const std::string& trigger)
	{
		auto words = string::explode(trigger, ' ');
		return words.size() == 1
			? checkTriggerWord(trigger)
			: checkTriggerPhrase(words)
			;
	}

	bool cbParser::checkTriggerWord(const std::string& trigger)
	{
		for (auto i = words.begin(); i != words.end(); ++i)
		{
			if (simplifyForTriggerMatch(*i) == trigger)
			{
				command_begin = i;
				command_end = i;
				return true;
			}
		}
		return false;
	}

	bool cbParser::checkTriggerPhrase(const std::vector<std::string>& trigger)
	{
		size_t trigger_i = 0;
		std::vector<std::string>::iterator begin;
		for (auto i = words.begin(); i != words.end(); ++i)
		{
			if (simplifyForTriggerMatch(*i) == trigger.at(trigger_i))
			{
				if (trigger_i == 0)
				{
					begin = i;
				}
				if (++trigger_i == trigger.size())
				{
					command_begin = begin;
					command_end = i;
					return true;
				}
			}
		}
		return false;
	}

	std::string cbParser::word2arg(std::vector<std::string>::iterator i) const noexcept
	{
		std::string arg = *i;
		if ((i + 1) == words.end())
		{
			prunePunctuation(arg);
		}
		return arg;
	}

	std::string cbParser::getArgWord() const noexcept
	{
		// assuming hasCommand() is true
		auto i = command_end;
		if (++i != words.end())
		{
			return word2arg(i);
		}
		return {};
	}

	std::string cbParser::getArgNumeric() const noexcept
	{
		// assuming hasCommand() is true
		for (auto i = command_end; ++i != words.end(); )
		{
			if (isNumericIgnorePunctuation(*i))
			{
				return word2arg(i);
			}
		}
		return {};
	}

	std::string cbParser::getArgNumericLefthand() const noexcept
	{
		// assuming hasCommand() is true
		auto i = command_begin;
		if (i != words.begin())
		{
			do
			{
				--i;
				if (string::isNumeric(*i))
				{
					return *i;
				}
			} while (i != words.begin());
		}
		return {};
	}
}
