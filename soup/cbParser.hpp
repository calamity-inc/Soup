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

		bool checkTriggerword(const std::string& triggerword);

		[[nodiscard]] std::string getArgWord() const noexcept;
	};
}
