#pragma once

#include "fwd.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace soup
{
	struct PhpState
	{
		std::filesystem::path cwd;

		[[nodiscard]] static const LangDesc& getLangDesc();

		static void processPhpmode(std::vector<Lexeme>& ls);

		[[nodiscard]] std::string evaluate(const std::string& code, unsigned int max_require_depth = 10) const;

		void execute(std::string& output, Reader& r, unsigned int max_require_depth = 10) const;
	};
}
