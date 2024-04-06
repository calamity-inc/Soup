#include "laMathFrontend.hpp"

#include "string.hpp"

namespace soup
{
	irModule laMathFrontend::parse(const std::string& program)
	{
		auto tks = tokenise(program);
		const Token* tk = tks.data();

		irModule m;
		irFunction fn;
		auto retn = soup::make_unique<irExpression>(IR_RET);
		if (auto e = expr(tk))
		{
			retn->children.emplace_back(std::move(e));
		}
		fn.insns.emplace_back(std::move(retn));
		m.func_exports.emplace_back(std::move(fn));
		return m;
	}

	std::vector<laMathFrontend::Token> laMathFrontend::tokenise(const std::string& code)
	{
		std::vector<Token> tks;
		for (auto i = code.begin(); i != code.end(); ++i)
		{
			if (string::isNumberChar(*i))
			{
				int64_t value = (*i - '0');
				while (++i != code.end() && string::isNumberChar(*i))
				{
					value *= 10;
					value += (*i - '0');
				}
				--i;
				tks.emplace_back(Token{ Token::T_VAL, value });
			}
			else if (*i == '+')
			{
				tks.emplace_back(Token{ Token::T_ADD });
			}
			else if (*i == '-')
			{
				tks.emplace_back(Token{ Token::T_SUB });
			}
			else if (*i == '*')
			{
				tks.emplace_back(Token{ Token::T_MUL });
			}
			else if (*i == '/')
			{
				tks.emplace_back(Token{ Token::T_DIV });
			}
			else if (*i == '%')
			{
				tks.emplace_back(Token{ Token::T_MOD });
			}
		}
		tks.emplace_back(Token{ Token::T_END });
		return tks;
	}

	uint8_t laMathFrontend::Token::getBinaryOperator() const noexcept
	{
		switch (type)
		{
		case T_ADD: return IR_ADD_I64;
		case T_SUB: return IR_SUB_I64;
		case T_MUL: return IR_MUL_I64;
		case T_DIV: return IR_SDIV_I64;
		case T_MOD: return IR_SMOD_I64;
		default:;
		}
		return 0xff;
	}

	[[nodiscard]] static uint8_t laMathGetPriority(uint8_t opr) noexcept
	{
		switch (opr)
		{
		case IR_MUL_I64: case IR_SDIV_I64: case IR_SMOD_I64: return 2;
		}
		return 1;
	}

	UniquePtr<irExpression> laMathFrontend::expr(const Token*& tk, uint8_t limit)
	{
		UniquePtr<irExpression> ret;
		if (tk->type == Token::T_VAL)
		{
			ret = soup::make_unique<irExpression>(IR_CONST_I64);
			ret->const_i64.value = tk->value;
			++tk;

			auto opr = tk->getBinaryOperator();
			while (opr != 0xff && laMathGetPriority(opr) > limit)
			{
				++tk;
				auto rhs = expr(tk, laMathGetPriority(opr));
				if (!rhs)
				{
					break;
				}
				ret = soup::make_unique<irExpression>(static_cast<irExpressionType>(opr), std::move(ret));
				ret->children.emplace_back(std::move(rhs));
				opr = tk->getBinaryOperator();
			}
		}
		return ret;
	}
}
