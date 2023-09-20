#pragma once

#include "fwd.hpp"

#include <vector>

#include "Token.hpp"

namespace soup
{
	class LangDesc
	{
	public:
		std::vector<Token> tokens{};
		bool semicolon_is_not_space = false;

		Token& addToken(const char* keyword, Token::parse_t parse = nullptr)
		{
			return addToken(keyword, Rgb::WHITE, parse);
		}

		Token& addToken(const char* keyword, Rgb colour, Token::parse_t parse = nullptr)
		{
			return tokens.emplace_back(Token{ keyword, colour, parse });
		}

		void addBlock(const char* start_keyword, const char* end_keyword);

		[[nodiscard]] std::vector<Lexeme> tokenise(const std::string& code) const;
		static void eraseNlTerminatedComments(std::vector<Lexeme>& ls, const ConstString& prefix);
		static void eraseSpace(std::vector<Lexeme>& ls);
		[[nodiscard]] astBlock parse(std::vector<Lexeme> ls) const;
		[[nodiscard]] astBlock parseImpl(std::vector<Lexeme>& ls) const;
		[[nodiscard]] astBlock parseNoCheck(std::vector<Lexeme>& ls) const;
	private:
		void parseBlock(ParserState& ps, const Token& t) const;
		void parseBlockRecurse(ParserState& ps, const Token& t, astBlock* b) const;

	public:
		[[nodiscard]] const Token& getToken(const char* keyword) const;
		[[nodiscard]] const Token& getToken(const Lexeme& l) const;

		[[nodiscard]] FormattedText highlightSyntax(const std::string& code) const;
		[[nodiscard]] FormattedText highlightSyntax(const std::vector<Lexeme>& ls) const;
	};
}
