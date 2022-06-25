#pragma once

#include "fwd.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace soup
{
	class PhpState
	{
	public:
		std::filesystem::path cwd;

		[[nodiscard]] static const Lexer& getLexer();

		[[nodiscard]] std::string evaluate(const std::string& code, unsigned int max_require_depth = 10) const;

		void execute(std::string& output, const std::vector<Op>& ops, unsigned int max_require_depth = 10) const;
	};
}
