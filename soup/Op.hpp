#pragma once

#include <map>
#include <vector>

#include "Lexeme.hpp"

namespace soup
{
	struct Op
	{
		int id;
		std::vector<Lexeme> args{};

		const Mixed& getArg(const std::map<std::string, Mixed>& vars, size_t idx) const
		{
			const Lexeme& tk = args.at(idx);
			if (tk.type == Lexeme::LITERAL)
			{
				return vars.at(tk.val.getString());
			}
			return tk.val;
		}
	};
}
