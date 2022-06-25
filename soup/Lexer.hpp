#pragma once

#include <unordered_map>
#include <vector>

#include "Lexeme.hpp"

namespace soup
{
	class Lexer
	{
	public:
		std::unordered_map<std::string, int> tokens{};

		void addToken(std::string keyword, int id);

		[[nodiscard]] std::vector<Lexeme> tokenise(const std::string& code) const;
	private:
		[[nodiscard]] static std::string getString(const std::string& code, std::string::const_iterator& i);

	public:
		[[nodiscard]] std::string getName(int id) const;
		[[nodiscard]] std::string getName(const Lexeme& l) const;
		[[nodiscard]] std::string getSourceString(const Lexeme& l) const;
		[[nodiscard]] std::string stringify(const std::vector<Lexeme>& ls) const;
	};
}
