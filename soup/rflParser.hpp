#pragma once

#include <string>
#include <vector>

#include "fwd.hpp"
#include "Lexeme.hpp"

namespace soup
{
	struct rflParser
	{
		std::vector<Lexeme> tks;
		std::vector<Lexeme>::iterator i;

		explicit rflParser(const std::string& code);

		[[nodiscard]] rflType readType();
		[[nodiscard]] rflVar readVar();
		void readVar(rflVar& var);
		[[nodiscard]] rflStruct readStruct();

		void align();
		void advance();
		[[nodiscard]] std::string readLiteral();
		[[nodiscard]] std::string peekLiteral();
	};
}
