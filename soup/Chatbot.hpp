#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "cbResult.hpp"

NAMESPACE_SOUP
{
	struct Chatbot
	{
		[[nodiscard]] static const std::vector<UniquePtr<cbCmd>>& getAllCommands();

		static void intialiseResources(std::filesystem::path dir);

		[[nodiscard]] static cbResult process(const std::string& str);
		[[nodiscard]] static std::string getResponse(const std::string& str);
	};
}
