#pragma once

#include <string>
#include <vector>

#include "cbMeasurement.hpp"

namespace soup
{
	class cbParser
	{
	private:
		std::string text;
		std::string::iterator command_begin;
		std::string::iterator command_end;

	public:
		explicit cbParser(std::string text)
			: text(std::move(text)), command_begin(text.end()), command_end(text.end())
		{
		}

		[[nodiscard]] bool hasCommand() const noexcept
		{
			return command_begin != text.end();
		}

	public:
		bool checkTrigger(const std::string& trigger);
		bool checkTriggers(const std::vector<std::string>& triggers);
		bool checkTriggerWord(const std::string& trigger);
		bool checkTriggerPhrase(const std::vector<std::string>& trigger);
		bool checkTriggerSymbol(const char trigger);
		bool checkTriggerSymbols(const char* triggers);

		void seekEndOfCurrentWord(std::string::iterator& i) const noexcept;
		void seekStartOfNextWord(std::string::iterator& i) const noexcept;
		void seekNextWord(std::string::iterator& i) const noexcept;
		void seekStartOfCurrentWord(std::string::iterator& i) const noexcept;
		[[nodiscard]] bool isAtStartOfWord(std::string::iterator i) const noexcept;
		void seekPreviousWord(std::string::iterator& i) const noexcept;

		[[nodiscard]] std::string::iterator getWordAfterCommandEnd() const noexcept;
		[[nodiscard]] std::string getWord(std::string::iterator i) const noexcept;
		[[nodiscard]] std::string getWord(std::string::iterator i, std::string::const_iterator end) const noexcept;

	public:
		[[nodiscard]] std::string getTrigger() const noexcept;

		[[nodiscard]] std::string getArgWord() const noexcept;
		[[nodiscard]] std::string getArgWordLefthand() const noexcept;

		[[nodiscard]] std::string getArgModifier() const noexcept;

		[[nodiscard]] std::string getArgNumeric() const noexcept;
		[[nodiscard]] std::string getArgNumericSecond() const noexcept;
		[[nodiscard]] std::string getArgNumericLefthand() const noexcept;

		[[nodiscard]] cbMeasurement getArgMeasurement() const noexcept;
		[[nodiscard]] cbMeasurement getArgMeasurementLefthand() const noexcept;

		[[nodiscard]] cbUnit getArgUnit() const noexcept;
	};
}
