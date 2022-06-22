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

		void flushLiteralBuffer()
		{
			if (lb.empty())
			{
				return;
			}
			auto tk = tkser->registry.find(lb);
			if (tk == tkser->registry.end())
			{
				std::string err = "Unregistered literal: ";
				err.append(lb);
				throw ParseError{ std::move(err) };
			}
			lb.clear();
			tokens.emplace_back(Token{ tk->second });
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
				st.tokens.emplace_back(Token{ Token::STRING, getString(code, i) });
				continue;
			}

			if (string::isSpace(*i))
			{
				st.flushLiteralBuffer();
			}
			else
			{
				st.lb.push_back(*i);
			}

			++i;
		}

		st.flushLiteralBuffer();

		return st.tokens;
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

	std::string Tokeniser::stringify(const std::vector<Token>& tks)
	{
		std::string output{};
		for (const auto& tk : tks)
		{
			output.push_back('[');
			output.append(std::to_string(tk.id));
			if (!tk.val.empty())
			{
				output.append(": ");
				output.append(tk.val);
			}
			output.push_back(']');
		}
		return output;
	}
}
