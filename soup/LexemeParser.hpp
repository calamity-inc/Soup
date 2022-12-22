#pragma once

#include <string>
#include <vector>

#include "fwd.hpp"
#include "Lexeme.hpp"

namespace soup
{
	struct LexemeParser
	{
		std::vector<Lexeme> tks;
		std::vector<Lexeme>::iterator i;

		explicit LexemeParser(const LangDesc& ld, const std::string& code);
		explicit LexemeParser(std::vector<Lexeme>&& tks);

		void advance()
		{
			++i;
		}
	};
}
