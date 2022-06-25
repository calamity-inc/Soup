#pragma once

#include "fwd.hpp"

#include <unordered_map>
#include <vector>

#include "Lexeme.hpp"
#include "Rgb.hpp"

namespace soup
{
	class Lexer
	{
	public:
		struct TokenExtra
		{
			Rgb colour;
		};

		std::unordered_map<std::string, int> tokens{};
		std::unordered_map<int, TokenExtra> extras{};

		void addToken(std::string keyword, int id);
		void addToken(std::string keyword, int id, Rgb colour);

		[[nodiscard]] std::vector<Lexeme> tokenise(const std::string& code) const;
	private:
		[[nodiscard]] static std::string getString(const std::string& code, std::string::const_iterator& i);

	public:
		[[nodiscard]] std::string getName(int id) const;
		[[nodiscard]] std::string getName(const Lexeme& l) const;
		[[nodiscard]] std::string getSourceString(const Lexeme& l) const;
		[[nodiscard]] std::string stringify(const std::vector<Lexeme>& ls) const;

		[[nodiscard]] Rgb getColour(int id) const;
		[[nodiscard]] Rgb getColour(const Lexeme& l) const;

		[[nodiscard]] FormattedText highlightSyntax(const std::string& code) const;
		[[nodiscard]] FormattedText highlightSyntax(const std::vector<Lexeme>& ls) const;
	};
}
