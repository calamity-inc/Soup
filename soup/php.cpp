#include "php.hpp"

#include "ParseError.hpp"
#include "Tokeniser.hpp"

namespace soup
{
	std::string php::evaluate(const std::string& code)
	{
		std::string output{};

		size_t plain_start = 0;
		for (auto i = code.begin(); i != code.end();)
		{
			if (*i == '<')
			{
				size_t token_start = (i - code.begin());
				if (*++i == '?'
					&& *++i == 'p'
					&& *++i == 'h'
					&& *++i == 'p'
					)
				{
					++i;
					output.append(code.data() + plain_start, token_start - plain_start);
					output.append(evaluatePhpmode(code, i));
					plain_start = (i - code.begin());
					continue;
				}
			}

			++i;
		}
		output.append(code.data() + plain_start);

		return output;
	}

	std::string php::evaluatePhpmode(const std::string& code, std::string::const_iterator& i)
	{
		size_t start = (i - code.begin());
		size_t code_end = code.length();

		for (; i != code.end(); ++i)
		{
			if (*i == '?'
				&& *++i == '>'
				)
			{
				code_end = (i - 1 - code.begin());
				++i;
				break;
			}
		}

		return evaluatePhp(code.substr(start, code_end - start));
	}

	enum PhpTokens : int
	{
		T_NOP = 0,

		T_ECHO = 100,
	};

	std::string php::evaluatePhp(const std::string& code)
	{
		std::string output{};
		try
		{
			Tokeniser tkser;
			tkser.addLiteral("echo", T_ECHO);
			tkser.addLiteral(";", T_NOP);
			auto tks = tkser.tokenise(code);

			for (auto i = tks.begin(); i != tks.end(); )
			{
				if (i->id == T_NOP)
				{
					i = tks.erase(i);
				}
				else
				{
					++i;
				}
			}

			for (auto i = tks.begin(); i != tks.end(); ++i)
			{
				if (i->id == T_ECHO)
				{
					if ((i + 1) != tks.end()
						&& (i + 1)->id == Token::STRING
						)
					{
						output.append((i + 1)->val);
					}
					else
					{
						throw ParseError("Unexpected echo");
					}
				}
			}
		}
		catch (const ParseError& e)
		{
			output = "ERROR: ";
			output.append(e.what());
		}
		return output;
	}
}
