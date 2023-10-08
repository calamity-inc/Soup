#include "cbParser.hpp"

#include "string.hpp"
#include "StringBuilder.hpp"

namespace soup
{
	static void prunePunctuation(std::string& str)
	{
		while (str.back() == '.'
			|| str.back() == '?'
			|| str.back() == '!'
			)
		{
			str.pop_back();
		}
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

	bool cbParser::checkTriggers(const std::vector<std::string>& triggers)
	{
		for (const auto& trigger : triggers)
		{
			if (checkTrigger(trigger))
			{
				return true;
			}
		}
		return false;
	}

	bool cbParser::checkTriggerWord(const std::string& trigger)
	{
		auto i = text.begin();

		StringBuilder buf;
		buf.beginCopy(text, i);

		for (;; ++i)
		{
			if (i == text.end() || *i == ' ')
			{
				buf.endCopy(text, i);

				if (simplifyForTriggerMatch(std::move(buf)) == trigger)
				{
					command_begin = i;
					command_end = i;
					seekStartOfCurrentWord(command_begin);
					seekEndOfCurrentWord(command_end);
					return true;
				}

				if (i == text.end())
				{
					break;
				}
				seekStartOfNextWord(i);
				buf.clear();
				buf.beginCopy(text, i);
			}
		}
		return false;
	}

	bool cbParser::checkTriggerPhrase(const std::vector<std::string>& trigger)
	{
		auto i = text.begin();
		size_t trigger_i = 0;
		std::string::iterator begin;

		StringBuilder buf;
		buf.beginCopy(text, i);

		for (;; ++i)
		{
			if (i == text.end() || *i == ' ')
			{
				buf.endCopy(text, i);

				if (simplifyForTriggerMatch(std::move(buf)) == trigger.at(trigger_i))
				{
					if (trigger_i == 0)
					{
						begin = i;
					}
					if (++trigger_i == trigger.size())
					{
						command_begin = begin;
						command_end = i;
						seekStartOfCurrentWord(command_begin);
						seekEndOfCurrentWord(command_end);
						return true;
					}
				}

				if (i == text.end())
				{
					break;
				}
				seekStartOfNextWord(i);
				buf.clear();
				buf.beginCopy(text, i);
			}
		}
		return false;
	}

	bool cbParser::checkTriggerSymbol(const char trigger)
	{
		for (auto i = text.begin(); i != text.end(); ++i)
		{
			if (*i == trigger)
			{
				command_begin = i;
				command_end = i + 1;
				return true;
			}
		}
		return false;
	}

	bool cbParser::checkTriggerSymbols(const char* triggers)
	{
		for (; *triggers; ++triggers)
		{
			if (checkTriggerSymbol(*triggers))
			{
				return true;
			}
		}
		return false;
	}

	void cbParser::seekEndOfCurrentWord(std::string::iterator& i) const noexcept
	{
		while (i != text.end() && *i != ' ')
		{
			++i;
		}
	}

	void cbParser::seekStartOfNextWord(std::string::iterator& i) const noexcept
	{
		while (i != text.end() && *i == ' ')
		{
			++i;
		}
	}

	void cbParser::seekNextWord(std::string::iterator& i) const noexcept
	{
		seekEndOfCurrentWord(i);
		seekStartOfNextWord(i);
	}

	void cbParser::seekStartOfCurrentWord(std::string::iterator& i) const noexcept
	{
		for (; i != text.begin(); --i)
		{
			if (isAtStartOfWord(i))
			{
				break;
			}
		}
	}

	bool cbParser::isAtStartOfWord(std::string::iterator i) const noexcept
	{
		return *(i - 1) == ' ';
	}

	void cbParser::seekPreviousWord(std::string::iterator& i) const noexcept
	{
		do
		{
			--i;
		} while (i != text.begin() && *i == ' ');
		seekStartOfCurrentWord(i);
	}

	std::string::iterator cbParser::getWordAfterCommandEnd() const noexcept
	{
		auto i = command_end;
		seekStartOfNextWord(i);
		return i;
	}

	std::string cbParser::getWord(std::string::iterator i) const noexcept
	{
		return getWord(i, text.end());
	}

	std::string cbParser::getWord(std::string::iterator i, std::string::const_iterator end) const noexcept
	{
		bool last_word = true;

		StringBuilder buf;
		buf.beginCopy(text, i);
		for (; i != end; ++i)
		{
			if (*i == ' ')
			{
				last_word = false;
				break;
			}
		}
		buf.endCopy(text, i);

		std::string arg = std::move(buf);
		if (last_word)
		{
			prunePunctuation(arg);
		}
		return arg;
	}

	std::string cbParser::getTrigger() const noexcept
	{
		return std::string(command_begin, command_end);
	}

	std::string cbParser::getArgWord() const noexcept
	{
		auto i = getWordAfterCommandEnd();
		if (i != text.end())
		{
			return getWord(i);
		}
		return {};
	}

	std::string cbParser::getArgWordLefthand() const noexcept
	{
		// assuming hasCommand() is true
		auto i = command_begin;
		if (i != text.begin())
		{
			seekPreviousWord(i);
			return getWord(i, command_begin);
		}
		return {};
	}

	std::string cbParser::getArgModifier() const noexcept
	{
		if (auto arg = getArgWordLefthand(); !arg.empty())
		{
			if (arg != "a"
				&& arg != "an"
				&& arg != "any"
				)
			{
				return arg;
			}
		}
		return {};
	}

	std::string cbParser::getArgNumeric() const noexcept
	{
		// assuming hasCommand() is true
		for (auto i = getWordAfterCommandEnd(); i != text.end(); seekNextWord(i))
		{
			auto arg = getWord(i);
			if (string::isNumeric(arg))
			{
				return arg;
			}
		}
		return {};
	}

	std::string cbParser::getArgNumericSecond() const noexcept
	{
		// assuming hasCommand() is true
		bool next = false;
		for (auto i = getWordAfterCommandEnd(); i != text.end(); seekNextWord(i))
		{
			auto arg = getWord(i);
			if (string::isNumeric(arg))
			{
				if (next)
				{
					return arg;
				}
				next = true;
			}
		}
		return {};
	}

	std::string cbParser::getArgNumericLefthand() const noexcept
	{
		// assuming hasCommand() is true
		for (auto i = command_begin; i != text.begin(); )
		{
			seekPreviousWord(i);
			auto arg = getWord(i, command_begin);
			if (string::isNumeric(arg))
			{
				return arg;
			}
		}
		return {};
	}

	cbMeasurement cbParser::getArgMeasurement() const noexcept
	{
		cbMeasurement m;
		for (auto i = getWordAfterCommandEnd(); i != text.end(); seekNextWord(i))
		{
			auto arg = getWord(i);
			if (string::isNumeric(arg))
			{
				char* str_end;
				m.quantity = std::strtod(arg.c_str(), &str_end);
				if (str_end == arg.c_str() || m.quantity == HUGE_VAL)
				{
					break;
				}
				seekNextWord(i);
				if (i != text.end())
				{
					m.unit = cbUnitFromString(getWord(i));
					return m;
				}
			}
		}
		return {};
	}

	cbMeasurement cbParser::getArgMeasurementLefthand() const noexcept
	{
		cbMeasurement m;
		for (auto i = command_begin; i != text.begin(); )
		{
			seekPreviousWord(i);
			m.unit = cbUnitFromString(getWord(i));
			if (m.unit != CB_NOUNIT
				&& i != text.begin()
				)
			{
				seekPreviousWord(i);
				auto arg = getWord(i);
				char* str_end;
				m.quantity = std::strtod(arg.c_str(), &str_end);
				if (str_end == arg.c_str() || m.quantity == HUGE_VAL)
				{
					break;
				}
				return m;
			}
		}
		return {};
	}

	cbUnit cbParser::getArgUnit() const noexcept
	{
		return cbUnitFromString(getArgWord());
	}

	std::string cbParser::getArgPhrase() const noexcept
	{
		// First, look if the user put anything into quotes.
		auto i = getWordAfterCommandEnd();
		while (i != text.end())
		{
			if (*i == '"' || *i == '\'')
			{
				const auto beg = i;
				const char quot = *i++;
				StringBuilder sb;
				sb.beginCopy(text, i);
				for (; i != text.end(); ++i)
				{
					if (*i == quot)
					{
						sb.endCopy(text, i);
						return sb;
					}
				}
				// Quote was not terminated, continue looking.
				i = beg;
			}
			seekNextWord(i);
		}

		// Nothing, use everything after trigger word.
		auto phrase = text.substr(getWordAfterCommandEnd() - text.begin());

		// Omit verbage, specifically for cbCmdQrcode.
		if (phrase.substr(0, 5) == "that ")
		{
			phrase.erase(0, 5);
			if (phrase.substr(0, 6) == "reads ")
			{
				phrase.erase(0, 6);
			}
			else if (phrase.substr(0, 5) == "says ")
			{
				phrase.erase(0, 5);
			}
			else if (phrase.substr(0, 9) == "contains ")
			{
				phrase.erase(0, 9);
			}
			else if (phrase.substr(0, 8) == "encodes ")
			{
				phrase.erase(0, 8);
			}
		}
		else
		{
			if (phrase.substr(0, 8) == "reading ")
			{
				phrase.erase(0, 8);
			}
			else if (phrase.substr(0, 7) == "saying ")
			{
				phrase.erase(0, 7);
			}
			else if (phrase.substr(0, 11) == "containing ")
			{
				phrase.erase(0, 11);
			}
			else if (phrase.substr(0, 9) == "encoding ")
			{
				phrase.erase(0, 9);
			}
		}

		return phrase;
	}
}
