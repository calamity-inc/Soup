#include "laPlutoFrontend.hpp"

#include "LangDesc.hpp"
#include "Lexeme.hpp"
#include "LexemeParser.hpp"

namespace soup
{
	static constexpr const char* TK_FUNCTION = "function";
	static constexpr const char* TK_RETURN = "return";
	static constexpr const char* TK_END = "end";
	static constexpr const char* TK_ADD = "+";
	static constexpr const char* TK_SUB = "-";
	static constexpr const char* TK_MUL = "*";
	static constexpr const char* TK_DIV = "/";
	static constexpr const char* TK_COMMA = ",";
	static constexpr const char* TK_LPAREN = "(";
	static constexpr const char* TK_RPAREN = ")";

	irModule laPlutoFrontend::parse(const std::string& program)
	{
		LangDesc ld;
		ld.addToken(TK_FUNCTION);
		ld.addToken(TK_RETURN);
		ld.addToken(TK_END);
		ld.addToken(TK_ADD);
		ld.addToken(TK_SUB);
		ld.addToken(TK_MUL);
		ld.addToken(TK_DIV);
		ld.addToken(TK_COMMA);
		ld.addToken(TK_LPAREN);
		ld.addToken(TK_RPAREN);

		auto ls = ld.tokenise(program);
		ld.eraseSpace(ls);
		LexemeParser lp(std::move(ls));

		irModule m;
		irModule::FuncExport top_level_fn;
		this->funcs = &m.func_exports;
		statlist(lp, m, top_level_fn);
		if (!top_level_fn.insns.empty())
		{
			//top_level_fn.name = "_start";
			m.func_exports.emplace_back(std::move(top_level_fn));
		}
		return m;
	}

	void laPlutoFrontend::statlist(LexemeParser& lp, irModule& m, irModule::FuncExport& fn)
	{
		while (lp.hasMore())
		{
			if (lp.i->token_keyword == TK_FUNCTION)
			{
				lp.advance();
				irModule::FuncExport inner_fn;
				if (lp.i->isLiteral())
				{
					locals.emplace();
					inner_fn.name = lp.i->getLiteral();
					lp.advance(); // skip name
					if (lp.hasMore()) { lp.advance(); } // skip '('
					while (lp.hasMore() && lp.i->isLiteral())
					{
						locals.top().emplace_back(lp.i->getLiteral());
						inner_fn.parameters.emplace_back(IR_I64);
						lp.advance(); // skip name
						if (lp.getTokenKeyword() != TK_COMMA)
						{
							break;
						}
						lp.advance(); // skip ','
					}
					if (lp.hasMore()) { lp.advance(); } // skip ')'
					if (lp.hasMore())
					{
						statlist(lp, m, inner_fn);
						if (lp.getTokenKeyword() == TK_END)
						{
							lp.advance();
						}
						m.func_exports.emplace_back(std::move(inner_fn));
					}
					locals.pop();
				}
			}
			else if (lp.i->token_keyword == TK_RETURN)
			{
				lp.advance();
				auto insn = soup::make_unique<irExpression>(IR_RET);
				do
				{
					auto val = expr(lp);
					if (!val)
					{
						break;
					}
					insn->children.emplace_back(std::move(val));
					fn.returns.emplace_back(IR_I64);
				} while (lp.getTokenKeyword() == TK_COMMA && (lp.advance(), true));
				fn.insns.emplace_back(std::move(insn));
			}
			else if (auto insn = exprstat(lp))
			{
				fn.insns.emplace_back(std::move(insn));
			}
			else
			{
				// unexpected top-level statement
				break;
			}
		}
	}

	[[nodiscard]] static uint8_t getbinopr(const char* token_keyword) noexcept
	{
		if (token_keyword == TK_ADD) { return IR_ADD; }
		if (token_keyword == TK_SUB) { return IR_SUB; }
		if (token_keyword == TK_MUL) { return IR_MUL; }
		if (token_keyword == TK_DIV) { return IR_SDIV; }
		return 0xff;
	}

	[[nodiscard]] static uint8_t getpriority(uint8_t opr) noexcept
	{
		switch (opr)
		{
		case IR_MUL: case IR_SDIV: return 2;
		}
		return 1;
	}

	UniquePtr<irExpression> laPlutoFrontend::expr(LexemeParser& lp, uint8_t limit)
	{
		auto ret = simpleexp(lp);
		if (ret)
		{
			auto opr = getbinopr(lp.getTokenKeyword());
			while (opr != 0xff && getpriority(opr) > limit)
			{
				lp.advance();
				auto rhs = expr(lp, getpriority(opr));
				if (!rhs)
				{
					break;
				}
				ret = soup::make_unique<irExpression>(static_cast<irExpressionType>(opr), std::move(ret));
				ret->children.emplace_back(std::move(rhs));
				opr = getbinopr(lp.getTokenKeyword());
			}
		}
		return ret;
	}

	UniquePtr<irExpression> laPlutoFrontend::simpleexp(LexemeParser& lp)
	{
		UniquePtr<irExpression> ret;
		if (lp.hasMore())
		{
			if (lp.i->token_keyword == Lexeme::VAL)
			{
				if (lp.i->val.isInt())
				{
					ret = soup::make_unique<irExpression>(IR_CONST_I64);
					ret->constant_value = lp.i->val.getInt();
					lp.advance();
				}
			}
			else if (lp.i->token_keyword == Lexeme::LITERAL)
			{
				if ((lp.i + 1) != lp.tks.end()
					&& (lp.i + 1)->token_keyword == TK_LPAREN
					)
				{
					ret = suffixedexp(lp);
				}
				else
				{
					const auto& locals_in_scope = locals.top();
					if (auto it = std::find(locals_in_scope.begin(), locals_in_scope.end(), lp.i->getLiteral()); it != locals_in_scope.end())
					{
						ret = soup::make_unique<irExpression>(IR_LOCAL);
						ret->index = static_cast<uint32_t>(std::distance(locals_in_scope.begin(), it));
						lp.advance();
					}
				}
			}
		}
		return ret;
	}

	UniquePtr<irExpression> laPlutoFrontend::exprstat(LexemeParser& lp)
	{
		return suffixedexp(lp);
	}

	UniquePtr<irExpression> laPlutoFrontend::suffixedexp(LexemeParser& lp)
	{
		UniquePtr<irExpression> ret;
		if (lp.getTokenKeyword() == Lexeme::LITERAL)
		{
			if ((lp.i + 1) != lp.tks.end()
				&& (lp.i + 1)->token_keyword == TK_LPAREN
				)
			{
				for (uint32_t i = 0; i != funcs->size(); ++i)
				{
					if ((*funcs)[i].name == lp.i->getLiteral())
					{
						ret = soup::make_unique<irExpression>(IR_CALL);
						ret->index = i;
						lp.advance(); // skip name
						if (lp.hasMore()) { lp.advance(); } // skip '('
						while (lp.hasMore())
						{
							auto arg = expr(lp);
							if (!arg)
							{
								break;
							}
							ret->children.emplace_back(std::move(arg));
							if (lp.getTokenKeyword() != TK_COMMA)
							{
								break;
							}
							lp.advance(); // skip ','
						}
						if (lp.hasMore()) { lp.advance(); } // skip ')'
						break;
					}
				}
			}
		}
		return ret;
	}
}
