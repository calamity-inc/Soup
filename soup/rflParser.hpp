#pragma once

#include "LexemeParser.hpp"

#include "fwd.hpp"

namespace soup
{
	struct rflParser : public LexemeParser
	{
		explicit rflParser(const std::string& code);

		[[nodiscard]] rflType readType();
		[[nodiscard]] rflVar readVar();
		void readVar(rflVar& var);
		[[nodiscard]] rflFunc readFunc();
		[[nodiscard]] rflStruct readStruct();

		void align();
		[[nodiscard]] std::string readLiteral();
		[[nodiscard]] std::string peekLiteral();
	};
}
