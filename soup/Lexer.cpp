#include "Lexer.hpp"

#include "ParseError.hpp"
#include "string.hpp"

namespace soup
{
	void Lexer::addToken(std::string keyword, int id)
	{
		tokens.emplace(std::move(keyword), id);
	}

	struct LexerState
	{
		const Lexer* const tkser;
		std::vector<Lexeme> lexemes{};
		std::string lb{};
		bool lb_is_space = false;

		void flushLiteralBuffer()
		{
			if (lb.empty())
			{
				return;
			}
			if (auto tk = tkser->tokens.find(lb); tk != tkser->tokens.end())
			{
				lexemes.emplace_back(Lexeme{ tk->second });
			}
			else if (auto opt = string::toInt<int64_t>(lb); opt.has_value())
			{
				lexemes.emplace_back(Lexeme{ Lexeme::VAL, opt.value()});
			}
			else if (lb_is_space)
			{
				lexemes.emplace_back(Lexeme{ Lexeme::SPACE, std::move(lb) });
			}
			else
			{
				lexemes.emplace_back(Lexeme{ Lexeme::LITERAL, std::move(lb) });
			}
			lb.clear();
			lb_is_space = false;
		}
	};

	std::vector<Lexeme> Lexer::tokenise(const std::string& code) const
	{
		LexerState st{ this };

		for (auto i = code.begin(); i != code.end(); )
		{
			if (*i == '"')
			{
				st.flushLiteralBuffer();
				++i;
				st.lexemes.emplace_back(Lexeme{ Lexeme::VAL, getString(code, i) });
				continue;
			}

			if (string::isSpace(*i)
				|| *i == ';'
				)
			{
				if (!st.lb_is_space)
				{
					st.flushLiteralBuffer();
					st.lb_is_space = true;
				}
			}
			else
			{
				if (st.lb_is_space
					|| *i == '('
					|| *i == '{'
					|| *i == '<'
					)
				{
					st.flushLiteralBuffer();
				}
			}
			st.lb.push_back(*i);
			if (*i == '>')
			{
				st.flushLiteralBuffer();
			}

			++i;
		}

		st.flushLiteralBuffer();

		return std::move(st.lexemes);
	}

	std::string Lexer::getString(const std::string& code, std::string::const_iterator& i)
	{
		size_t start = (i - code.begin());

		for (; *i != '"'; ++i)
		{
			if (i == code.end())
			{
				throw ParseError("Unterminated string");
			}
		}

		return code.substr(start, i++ - code.begin() - start);
	}

	std::string Lexer::getName(int id) const
	{
		switch (id)
		{
		case Lexeme::VAL: return "value";
		case Lexeme::LITERAL: return "literal";
		case Lexeme::SPACE: return "space";
		}
		for (const auto& e : tokens)
		{
			if (e.second == id)
			{
				return e.first;
			}
		}
		return std::to_string(id);
	}

	std::string Lexer::getName(const Lexeme& l) const
	{
		if (l.type != Lexeme::SPACE)
		{
			std::string str = l.val.toString();
			if (!str.empty())
			{
				str.insert(0, 1, ' ');
				str.insert(0, getName(l.type));
				return str;
			}
		}
		return getName(l.type);
	}

	std::string Lexer::getSourceString(const Lexeme& l) const
	{
		for (const auto& e : tokens)
		{
			if (e.second == l.type)
			{
				return e.first;
			}
		}
		return l.val.toString();
	}

	std::string Lexer::stringify(const std::vector<Lexeme>& ls) const
	{
		std::string output{};
		for (const auto& l : ls)
		{
			output.push_back('[');
			output.append(getName(l));
			output.push_back(']');
		}
		return output;
	}
}
