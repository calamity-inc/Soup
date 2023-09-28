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

	bool cbParser::checkTriggerWord(const std::string& trigger)
	{
		auto i = text.begin();

		StringBuilder buf;
		buf.beginCopy(text, i);

		for (;; ++i)
		{
			if (*i == ' ' || i == text.end())
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
			if (*i == ' ' || i == text.end())
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
			if (*(i - 1) == ' ')
			{
				break;
			}
		}
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
		bool last_word = true;

		StringBuilder buf;
		buf.beginCopy(text, i);
		for (; i != text.end(); ++i)
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
			return getWord(i);
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
			auto arg = getWord(i);
			if (string::isNumeric(arg))
			{
				return arg;
			}
		}
		return {};
	}

	cbMeasurement cbParser::getArgMeasurement() const noexcept
	{
		try
		{
			cbMeasurement m;
			for (auto i = getWordAfterCommandEnd(); i != text.end(); seekNextWord(i))
			{
				auto arg = getWord(i);
				if (string::isNumeric(arg))
				{
					m.quantity = std::stod(arg);
					seekNextWord(i);
					if (i != text.end())
					{
						m.unit = cbUnitFromString(getWord(i));
						return m;
					}
				}
			}
		}
		catch (...)
		{
		}
		return {};
	}

	cbMeasurement cbParser::getArgMeasurementLefthand() const noexcept
	{
		try
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
					m.quantity = std::stod(getWord(i));
					return m;
				}
			}
		}
		catch (...)
		{
		}
		return {};
	}

	cbUnit cbParser::getArgUnit() const noexcept
	{
		return cbUnitFromString(getArgWord());
	}
}
