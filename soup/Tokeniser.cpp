#include "Tokeniser.hpp"

#include "ParseError.hpp"
#include "string.hpp"

namespace soup
{
	void Tokeniser::addLiteral(std::string keyword, int id)
	{
		registry.emplace(std::move(keyword), id);
	}

	struct TokeniserState
	{
		const Tokeniser* const tkser;
		std::vector<Token> tokens{};
		std::string lb{};
		bool lb_is_space = false;

		void flushLiteralBuffer()
		{
			if (lb.empty())
			{
				return;
			}
			if (auto tk = tkser->registry.find(lb); tk != tkser->registry.end())
			{
				tokens.emplace_back(Token{ tk->second });
			}
			else if (auto opt = string::toInt<int64_t>(lb); opt.has_value())
			{
				tokens.emplace_back(Token{ Token::VAL, opt.value()});
			}
			else if (lb_is_space)
			{
				tokens.emplace_back(Token{ Token::SPACE, std::move(lb) });
			}
			else
			{
				tokens.emplace_back(Token{ Token::LITERAL, std::move(lb) });
			}
			lb.clear();
			lb_is_space = false;
		}
	};

	std::vector<Token> Tokeniser::tokenise(const std::string& code) const
	{
		TokeniserState st{ this };

		for (auto i = code.begin(); i != code.end(); )
		{
			if (*i == '"')
			{
				st.flushLiteralBuffer();
				++i;
				st.tokens.emplace_back(Token{ Token::VAL, getString(code, i) });
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

		return std::move(st.tokens);
	}

	std::string Tokeniser::getString(const std::string& code, std::string::const_iterator& i)
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

	std::string Tokeniser::getName(int id) const
	{
		switch (id)
		{
		case Token::VAL: return "value";
		case Token::LITERAL: return "literal";
		case Token::SPACE: return "space";
		}
		for (const auto& e : registry)
		{
			if (e.second == id)
			{
				return e.first;
			}
		}
		return std::to_string(id);
	}

	std::string Tokeniser::getName(const Token& tk) const
	{
		if (tk.id != Token::SPACE)
		{
			std::string str = tk.val.toString();
			if (!str.empty())
			{
				str.insert(0, 1, ' ');
				str.insert(0, getName(tk.id));
				return str;
			}
		}
		return getName(tk.id);
	}

	std::string Tokeniser::getSourceString(const Token& tk)
	{
		return tk.val.toString();
	}

	std::string Tokeniser::stringify(const std::vector<Token>& tks) const
	{
		std::string output{};
		for (const auto& tk : tks)
		{
			output.push_back('[');
			output.append(getName(tk));
			output.push_back(']');
		}
		return output;
	}
}
