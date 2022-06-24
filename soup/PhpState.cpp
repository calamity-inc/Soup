#include "PhpState.hpp"

#include "Op.hpp"
#include "ParseError.hpp"
#include "string.hpp"
#include "Tokeniser.hpp"

namespace soup
{
	enum PhpTokens : int
	{
		T_PHPMODE_START = 0,
		T_PHPMODE_END,
		T_SET,
		T_ECHO,
		T_REQUIRE,
		T_FUNC,
		T_BLOCK_START,
		T_BLOCK_END,

		OP_CALL,
	};

	[[nodiscard]] static constexpr bool isValidArg(int id) noexcept
	{
		return id == Token::VAL
			|| id == Token::LITERAL
			;
	}

	[[nodiscard]] static Op collapse_lr(const Tokeniser& tkser, std::vector<Token>& tks, std::vector<Token>::iterator& i)
	{
		Token tk = std::move(*i);
		if (i == tks.begin())
		{
			std::string err = tkser.getName(tk);
			err.append(" expected lefthand argument, found start of code");
			throw ParseError(std::move(err));
		}
		Op op{ tk.id };
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
		--i;
		i = tks.erase(i);
		i = tks.erase(i);
		return op;
	}

	[[nodiscard]] static Op collapse_l(const Tokeniser& tkser, std::vector<Token>& tks, std::vector<Token>::iterator& i)
	{
		Token tk = std::move(*i);
		if (i == tks.begin())
		{
			std::string err = tkser.getName(tk);
			err.append(" expected lefthand argument, found start of code");
			throw ParseError(std::move(err));
		}
		Op op{ tk.id };
		i = tks.erase(i);
		if (!isValidArg((i - 1)->id))
		{
			std::string err = tkser.getName(tk);
			err.append(" expected lefthand argument, found ");
			err.append(tkser.getName(*i));
			throw ParseError(std::move(err));
		}
		op.args.emplace_back(std::move(*(i - 1)));
		--i;
		i = tks.erase(i);
		return op;
	}

	[[nodiscard]] static Op collapse_r(const Tokeniser& tkser, std::vector<Token>& tks, std::vector<Token>::iterator& i)
	{
		Token tk = std::move(*i);
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
		Op op{ tk.id, { std::move(*i) } };
		i = tks.erase(i);
		return op;
	}

	static bool processBlockTokens(const Tokeniser& tkser, std::vector<Token>& tks, std::vector<Token>::iterator& i, std::vector<Op>& ops)
	{
		for (; i != tks.end(); )
		{
			switch (i->id)
			{
			case Token::LITERAL:
				if (i->val.getString() == "()")
				{
					Op op = collapse_l(tkser, tks, i);
					op.id = OP_CALL;
					ops.emplace_back(std::move(op));
					break;
				}
				[[fallthrough]];
			default:
				++i;
				break;

			case T_SET:
				ops.emplace_back(collapse_lr(tkser, tks, i));
				break;

			case T_ECHO:
			case T_REQUIRE:
				ops.emplace_back(collapse_r(tkser, tks, i));
				break;

			case T_BLOCK_END:
				i = tks.erase(i);
				return true;
			}
		}
		return false;
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

	std::string PhpState::evaluate(const std::string& code, unsigned int max_require_depth) const
	{
		std::string output{};
		try
		{
			Tokeniser tkser;
			tkser.addLiteral("<?php", T_PHPMODE_START);
			tkser.addLiteral("?>", T_PHPMODE_END);
			tkser.addLiteral("=", T_SET);
			tkser.addLiteral("echo", T_ECHO);
			tkser.addLiteral("require", T_REQUIRE);
			tkser.addLiteral("function", T_FUNC);
			tkser.addLiteral("{", T_BLOCK_START);
			tkser.addLiteral("}", T_BLOCK_END);
			auto tks = tkser.tokenise(code);

#if DEBUG_PARSING
			output = "Tokenised: ";
			output.append(tkser.stringify(tks));
#endif

			std::string non_phpmode_buffer{};
			for (auto i = tks.begin(); i != tks.end(); )
			{
				if (i->id == T_PHPMODE_START)
				{
					i = tks.erase(i);
					if (!non_phpmode_buffer.empty())
					{
						i = tks.insert(i, Token{ Token::VAL, std::move(non_phpmode_buffer) });
						i = tks.insert(i, Token{ T_ECHO });
						non_phpmode_buffer.clear();
					}
					for (; i != tks.end() && i->id != T_PHPMODE_END; )
					{
						if (i->id == Token::SPACE)
						{
							i = tks.erase(i);
						}
						else
						{
							++i;
						}
					}
				}
				else
				{
					non_phpmode_buffer.append(tkser.getSourceString(*i));
					i = tks.erase(i);
				}
			}

#if DEBUG_PARSING
			output.append("\nOutside phpmode squashed: ");
			output.append(tkser.stringify(tks));
#endif

			for (auto i = tks.begin(); i != tks.end(); )
			{
				if (i->id != T_FUNC)
				{
					++i;
					continue;
				}
				i = tks.erase(i);
				if (i == tks.end()
					|| i->id != Token::LITERAL
					|| i->val.getString() != "()"
					)
				{
					std::string msg = "Expected '()' after 'function', found ";
					msg.append(tkser.getName(*i));
					throw ParseError(std::move(msg));
				}
				i = tks.erase(i);
				if (i == tks.end()
					|| i->id != T_BLOCK_START
					)
				{
					std::string msg = "Expected block start after function(), found ";
					msg.append(tkser.getName(*i));
					throw ParseError(std::move(msg));
				}
				i = tks.erase(i);
				size_t start = (i - tks.begin());
				std::vector<Op> ops{};
				if (!processBlockTokens(tkser, tks, i, ops))
				{
					throw ParseError("Expected block end, found end of code");
				}
				if ((i - tks.begin()) != start)
				{
					std::string err = "Unexpected ";
					err.append(tkser.getName(*i));
					throw ParseError(std::move(err));
				}
				i = tks.insert(i, Token{ Token::VAL, std::move(ops) });
			}

#if DEBUG_PARSING
			output.append("\nFunctions squashed: ");
			output.append(tkser.stringify(tks));
#endif

			auto i = tks.begin();
			std::vector<Op> ops{};
			if (processBlockTokens(tkser, tks, i, ops))
			{
				throw ParseError("Unexpected block end; no matching block start");
			}

#if DEBUG_PARSING
			output.append("\nOps: ");
			output.append(stringifyOps(tkser, ops));
			output.push_back('\n');
#endif

			for (const auto& tk : tks)
			{
				std::string err = "Unexpected ";
				err.append(tkser.getName(tk));
				throw ParseError(std::move(err));
			}

			execute(output, ops, max_require_depth);
		}
		catch (const std::runtime_error& e)
		{
			if (!output.empty())
			{
				output.push_back('\n');
			}
			output.append("ERROR: ");
			output.append(e.what());
		}
		return output;
	}

	void PhpState::execute(std::string& output, const std::vector<Op>& ops, unsigned int max_require_depth) const
	{
		std::map<std::string, Mixed> vars{};
		for (const auto& op : ops)
		{
			switch (op.id)
			{
			case T_SET:
			{
				std::string& key = op.args.at(0).val.getString();
				const Mixed& val = op.args.at(1).val;

				if (auto e = vars.find(key); e != vars.end())
				{
					e->second = std::move(val);
				}
				else
				{
					vars.emplace(std::move(key), val);
				}
			}
			break;

			case T_ECHO:
				output.append(op.getArg(vars, 0).toString());
				break;

			case T_REQUIRE:
			{
				if (max_require_depth == 0)
				{
					throw std::runtime_error("Max require depth exceeded");
				}
				std::filesystem::path file = cwd;
				file /= op.getArg(vars, 0).toString();
				if (!std::filesystem::exists(file))
				{
					std::string err = "Required file doesn't exist: ";
					err.append(file.string());
					throw std::runtime_error(std::move(err));
				}
				output.append(evaluate(string::fromFile(file.string()), max_require_depth - 1));
			}
			break;

			case OP_CALL:
				execute(output, op.getArg(vars, 0).getOpArray(), max_require_depth);
				break;
			}
		}
	}
}
