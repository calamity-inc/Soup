#pragma once

#include <string>
#include <vector>

namespace soup
{
	struct cbParser
	{
		std::vector<std::string> words;
		std::vector<std::string>::iterator command;

		explicit cbParser(const std::string& text);

		[[nodiscard]] bool hasCommand() const noexcept
		{
			return command != words.end();
		}

		bool checkTrigger(const std::string& trigger);

		[[nodiscard]] std::string getArgWord() const noexcept;
	};
}
