#include "PhpState.hpp"

#include "Op.hpp"
#include "ParseError.hpp"
#include "string.hpp"
#include "Lexer.hpp"

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

	[[nodiscard]] static Lexer getLexerImpl()
	{
		Lexer lexer;
		lexer.addToken("<?php", T_PHPMODE_START);
		lexer.addToken("?>", T_PHPMODE_END);
		lexer.addToken("=", T_SET, Rgb::RED);
		lexer.addToken("echo", T_ECHO, Rgb::BLUE);
		lexer.addToken("require", T_REQUIRE, Rgb::RED);
		lexer.addToken("function", T_FUNC, Rgb::BLUE);
		lexer.addToken("{", T_BLOCK_START);
		lexer.addToken("}", T_BLOCK_END);
		return lexer;
	}

	const Lexer& PhpState::getLexer()
	{
		static Lexer lexer = getLexerImpl();
		return lexer;
	}

	[[nodiscard]] static constexpr bool isValidArg(int id) noexcept
	{
		return id == Lexeme::VAL
			|| id == Lexeme::LITERAL
			;
	}

	[[nodiscard]] static Op collapse_lr(const Lexer& lexer, std::vector<Lexeme>& ls, std::vector<Lexeme>::iterator& i)
	{
		Lexeme l = std::move(*i);
		if (i == ls.begin())
		{
			std::string err = lexer.getName(l);
			err.append(" expected lefthand argument, found start of code");
			throw ParseError(std::move(err));
		}
		Op op{ l.type };
		op.args.reserve(2);
		i = ls.erase(i);
		if (i == ls.end())
		{
			std::string err = lexer.getName(l);
			err.append(" expected righthand argument, found end of code");
			throw ParseError(std::move(err));
		}
		if (!isValidArg((i - 1)->type))
		{
			std::string err = lexer.getName(l);
			err.append(" expected lefthand argument, found ");
			err.append(lexer.getName(*i));
			throw ParseError(std::move(err));
		}
		if (!isValidArg(i->type))
		{
			std::string err = lexer.getName(l);
			err.append(" expected righthand argument, found ");
			err.append(lexer.getName(*i));
			throw ParseError(std::move(err));
		}
		op.args.emplace_back(std::move(*(i - 1)));
		op.args.emplace_back(std::move(*i));
		--i;
		i = ls.erase(i);
		i = ls.erase(i);
		return op;
	}

	[[nodiscard]] static Op collapse_l(const Lexer& lexer, std::vector<Lexeme>& ls, std::vector<Lexeme>::iterator& i)
	{
		Lexeme l = std::move(*i);
		if (i == ls.begin())
		{
			std::string err = lexer.getName(l);
			err.append(" expected lefthand argument, found start of code");
			throw ParseError(std::move(err));
		}
		Op op{ l.type };
		i = ls.erase(i);
		if (!isValidArg((i - 1)->type))
		{
			std::string err = lexer.getName(l);
			err.append(" expected lefthand argument, found ");
			err.append(lexer.getName(*i));
			throw ParseError(std::move(err));
		}
		op.args.emplace_back(std::move(*(i - 1)));
		--i;
		i = ls.erase(i);
		return op;
	}

	[[nodiscard]] static Op collapse_r(const Lexer& lexer, std::vector<Lexeme>& ls, std::vector<Lexeme>::iterator& i)
	{
		Lexeme l = std::move(*i);
		i = ls.erase(i);
		if (i == ls.end())
		{
			std::string err = lexer.getName(l);
			err.append(" expected righthand argument, found end of code");
			throw ParseError(std::move(err));
		}
		if (!isValidArg(i->type))
		{
			std::string err = lexer.getName(l);
			err.append(" expected righthand argument, found ");
			err.append(lexer.getName(*i));
			throw ParseError(std::move(err));
		}
		Op op{ l.type, { std::move(*i) } };
		i = ls.erase(i);
		return op;
	}

	static bool processBlockTokens(const Lexer& lexer, std::vector<Lexeme>& ls, std::vector<Lexeme>::iterator& i, std::vector<Op>& ops)
	{
		for (; i != ls.end(); )
		{
			switch (i->type)
			{
			case Lexeme::LITERAL:
				if (i->val.getString() == "()")
				{
					Op op = collapse_l(lexer, ls, i);
					op.id = OP_CALL;
					ops.emplace_back(std::move(op));
					break;
				}
				[[fallthrough]];
			default:
				++i;
				break;

			case T_SET:
				ops.emplace_back(collapse_lr(lexer, ls, i));
				break;

			case T_ECHO:
			case T_REQUIRE:
				ops.emplace_back(collapse_r(lexer, ls, i));
				break;

			case T_BLOCK_END:
				i = ls.erase(i);
				return true;
			}
		}
		return false;
	}
	
#define DEBUG_PARSING false

#if DEBUG_PARSING
	[[nodiscard]] static std::string stringifyOps(const Lexer& lexer, const std::vector<Op>& ops)
	{
		std::string str{};
		for (auto i = ops.begin(); i != ops.end(); ++i)
		{
			str.push_back('{');
			str.append(lexer.getName(i->id));
			if (!i->args.empty())
			{
				str.append(": ");
				for (const auto& arg : i->args)
				{
					str.push_back('[');
					str.append(lexer.getName(arg));
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
			const Lexer& lexer = getLexer();
			auto ls = lexer.tokenise(code);

#if DEBUG_PARSING
			output = "Tokenised: ";
			output.append(lexer.stringify(ls));
#endif

			std::string non_phpmode_buffer{};
			for (auto i = ls.begin(); i != ls.end(); )
			{
				if (i->type == T_PHPMODE_START)
				{
					i = ls.erase(i);
					if (!non_phpmode_buffer.empty())
					{
						i = ls.insert(i, Lexeme{ Lexeme::VAL, std::move(non_phpmode_buffer) });
						i = ls.insert(i, Lexeme{ T_ECHO });
						non_phpmode_buffer.clear();
					}
					for (; i != ls.end(); )
					{
						if (i->type == T_PHPMODE_END)
						{
							i = ls.erase(i);
							break;
						}
						if (i->type == Lexeme::SPACE)
						{
							i = ls.erase(i);
						}
						else
						{
							++i;
						}
					}
				}
				else
				{
					non_phpmode_buffer.append(lexer.getSourceString(*i));
					i = ls.erase(i);
				}
			}

#if DEBUG_PARSING
			output.append("\nOutside phpmode squashed: ");
			output.append(lexer.stringify(ls));
#endif

			for (auto i = ls.begin(); i != ls.end(); )
			{
				if (i->type != T_FUNC)
				{
					++i;
					continue;
				}
				i = ls.erase(i);
				if (i == ls.end()
					|| i->type != Lexeme::LITERAL
					|| i->val.getString() != "()"
					)
				{
					std::string msg = "Expected '()' after 'function', found ";
					msg.append(lexer.getName(*i));
					throw ParseError(std::move(msg));
				}
				i = ls.erase(i);
				if (i == ls.end()
					|| i->type != T_BLOCK_START
					)
				{
					std::string msg = "Expected block start after function(), found ";
					msg.append(lexer.getName(*i));
					throw ParseError(std::move(msg));
				}
				i = ls.erase(i);
				size_t start = (i - ls.begin());
				std::vector<Op> ops{};
				if (!processBlockTokens(lexer, ls, i, ops))
				{
					throw ParseError("Expected block end, found end of code");
				}
				if ((i - ls.begin()) != start)
				{
					std::string err = "Unexpected ";
					err.append(lexer.getName(*i));
					throw ParseError(std::move(err));
				}
				i = ls.insert(i, Lexeme{ Lexeme::VAL, std::move(ops) });
			}

#if DEBUG_PARSING
			output.append("\nFunctions squashed: ");
			output.append(lexer.stringify(ls));
#endif

			auto i = ls.begin();
			std::vector<Op> ops{};
			if (processBlockTokens(lexer, ls, i, ops))
			{
				throw ParseError("Unexpected block end; no matching block start");
			}

#if DEBUG_PARSING
			output.append("\nOps: ");
			output.append(stringifyOps(lexer, ops));
			output.push_back('\n');
#endif

			for (const auto& tk : ls)
			{
				std::string err = "Unexpected ";
				err.append(lexer.getName(tk));
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
