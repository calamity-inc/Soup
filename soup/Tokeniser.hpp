#pragma once

#include <unordered_map>
#include <vector>

#include "Token.hpp"

namespace soup
{
	class Tokeniser
	{
	public:
		std::unordered_map<std::string, int> registry{};

		void addLiteral(std::string keyword, int id);

		[[nodiscard]] std::vector<Token> tokenise(const std::string& code) const;
	private:
		[[nodiscard]] static std::string getString(const std::string& code, std::string::const_iterator& i);

	public:
		[[nodiscard]] std::string getName(int id) const;
		[[nodiscard]] std::string getName(const Token& tk) const;
		[[nodiscard]] std::string stringify(const std::vector<Token>& tks) const;
	};
}
