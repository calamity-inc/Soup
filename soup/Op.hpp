#pragma once

#include <map>
#include <vector>

#include "Token.hpp"

namespace soup
{
	struct Op
	{
		int id;
		std::vector<Token> args{};

		const Mixed& getArg(const std::map<std::string, Mixed>& vars, size_t idx) const
		{
			const Token& tk = args.at(idx);
			if (tk.id == Token::LITERAL)
			{
				return vars.at(tk.val.getString());
			}
			return tk.val;
		}
	};
}
