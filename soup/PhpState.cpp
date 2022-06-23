#include "PhpState.hpp"

#include <map>

#include "ParseError.hpp"
#include "string.hpp"
#include "Tokeniser.hpp"

namespace soup
{
	std::string PhpState::evaluate(const std::string& code) const
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

	std::string PhpState::evaluatePhpmode(const std::string& code, std::string::const_iterator& i) const
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
		T_SET = 0,
		T_ECHO,
		T_REQUIRE,
	};

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

	[[nodiscard]] static constexpr bool isValidArg(int id) noexcept
	{
		return id == Token::VAL || id == Token::LITERAL;
	}

	static void collapse_lr(const Tokeniser& tkser, std::vector<Token>& tks, std::vector<Op>& ops, int id)
	{
		for (auto i = tks.begin(); i != tks.end(); )
		{
			if (i->id != id)
			{
				++i;
				continue;
			}
			Token tk = *i;
			if (i == tks.begin())
			{
				std::string err = tkser.getName(tk);
				err.append(" expected lefthand argument, found start of code");
				throw ParseError(std::move(err));
			}
			Op op{ id };
			op.args.reserve(2);
			i = tks.erase(i);
			if (i == tks.end())
			{
				std::string err = tkser.getName(tk);
				err.append(" expected righthand argument, found end of code");
				throw ParseError(std::move(err));
			}
			if (!isValidArg((i - 1)->id))
			{
				std::string err = tkser.getName(tk);
				err.append(" expected lefthand argument, found ");
				err.append(tkser.getName(*i));
				throw ParseError(std::move(err));
			}
			if (!isValidArg(i->id))
			{
				std::string err = tkser.getName(tk);
				err.append(" expected righthand argument, found ");
				err.append(tkser.getName(*i));
				throw ParseError(std::move(err));
			}
			op.args.emplace_back(std::move(*(i - 1)));
			op.args.emplace_back(std::move(*i));
			ops.emplace_back(std::move(op));
			--i;
			i = tks.erase(i);
			i = tks.erase(i);
		}
	}

	static void collapse_r(const Tokeniser& tkser, std::vector<Token>& tks, std::vector<Op>& ops, int id)
	{
		for (auto i = tks.begin(); i != tks.end(); )
		{
			if (i->id != id)
			{
				++i;
				continue;
			}
			Token tk = *i;
			i = tks.erase(i);
			if (i == tks.end())
			{
				std::string err = tkser.getName(tk);
				err.append(" expected righthand argument, found end of code");
				throw ParseError(std::move(err));
			}
			if (!isValidArg(i->id))
			{
				std::string err = tkser.getName(tk);
				err.append(" expected righthand argument, found ");
				err.append(tkser.getName(*i));
				throw ParseError(std::move(err));
			}
			ops.emplace_back(Op{ id, { std::move(*i) } });
			i = tks.erase(i);
		}
	}

#define DEBUG_PARSING false

#if DEBUG_PARSING
	[[nodiscard]] static std::string stringifyOps(const Tokeniser& tkser, const std::vector<Op>& ops)
	{
		std::string str{};
		for (auto i = ops.begin(); i != ops.end(); ++i)
		{
			str.push_back('{');
			str.append(tkser.getName(i->id));
			if (!i->args.empty())
			{
				str.append(": ");
				for (const auto& arg : i->args)
				{
					str.push_back('[');
					str.append(tkser.getName(arg));
					str.push_back(']');
				}
			}
			str.push_back('}');
		}
		return str;
	}
#endif

	std::string PhpState::evaluatePhp(const std::string& code) const
	{
		std::string output{};
		try
		{
			Tokeniser tkser;
			tkser.addLiteral("=", T_SET);
			tkser.addLiteral("echo", T_ECHO);
			tkser.addLiteral("require", T_REQUIRE);
			auto tks = tkser.tokenise(code);

			std::vector<Op> ops{};

#if DEBUG_PARSING
			output = "Tokens: ";
			output.append(tkser.stringify(tks));
#endif

			collapse_lr(tkser, tks, ops, T_SET);
			collapse_r(tkser, tks, ops, T_ECHO);
			collapse_r(tkser, tks, ops, T_REQUIRE);

			for (const auto& tk : tks)
			{
				std::string err = "Unexpected ";
				err.append(tkser.getName(tk));
				throw ParseError(std::move(err));
			}

#if DEBUG_PARSING
			output.append("\nOps: ");
			output.append(stringifyOps(tkser, ops));
#endif
			
			std::map<std::string, Mixed> vars{};
			for (const auto& op : ops)
			{
				switch (op.id)
				{
				case T_SET:
					vars.emplace(std::move(op.args.at(0).val.getString()), std::move(op.args.at(1).val));
					break;

				case T_ECHO:
					output.append(op.getArg(vars, 0).toString());
					break;

				case T_REQUIRE:
					{
						std::filesystem::path file = cwd;
						file /= op.getArg(vars, 0).toString();
						if (!std::filesystem::exists(file))
						{
							std::string err = "Required file doesn't exist: ";
							err.append(file.string());
							throw std::runtime_error(std::move(err));
						}
						output.append(evaluate(string::fromFile(file.string())));
					}
					break;
				}
			}
		}
		catch (const std::runtime_error& e)
		{
			output = "ERROR: ";
			output.append(e.what());
		}
		return output;
	}
}
