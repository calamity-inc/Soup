#pragma once

#include <string>
#include <vector>

namespace soup
{
	class cbParser
	{
	public:
		std::vector<std::string> words;
		std::vector<std::string>::iterator command_begin;
		std::vector<std::string>::iterator command_end;

		explicit cbParser(const std::string& text);

		[[nodiscard]] bool hasCommand() const noexcept
		{
			return command_begin != words.end();
		}

		bool checkTrigger(const std::string& trigger);
	private:
		bool checkTriggerWord(const std::string& trigger);
		bool checkTriggerPhrase(const std::vector<std::string>& trigger);

	public:
		[[nodiscard]] std::string getArgWord() const noexcept;
	};
}
