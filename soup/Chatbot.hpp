#pragma once

#include "fwd.hpp"

#include <string>
#include <vector>

namespace soup
{
	struct Chatbot
	{
		[[nodiscard]] static const std::vector<UniquePtr<cbCmd>>& getAllCommands();

		[[nodiscard]] static cbResult process(const std::string& text);
		[[nodiscard]] static std::string getResponse(const std::string& text);
	};
}
