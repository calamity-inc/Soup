#include "LangDesc.hpp"

#include "FormattedText.hpp"
#include "Lexeme.hpp"
#include "ParseError.hpp"
#include "ParserState.hpp"
#include "parse_tree.hpp"

#define DEBUG_PARSE false

#if DEBUG_PARSE
#include <iostream>
#endif

namespace soup
{
	[[nodiscard]] static std::string getString(const std::string& code, std::string::const_iterator& i)
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

	void LangDesc::addBlock(const char* start_keyword, const char* end_keyword)
	{
		addToken(start_keyword, [](ParserState& ps)
		{
			ps.pushLefthandNode(ps.collapseRighthandBlock(reinterpret_cast<const char*>(ps.getToken().user_data)));
		}).user_data = reinterpret_cast<uintptr_t>(end_keyword);
		addToken(end_keyword);
	}

	struct LexerState
	{
		const LangDesc* const ld;
		std::vector<Lexeme> lexemes{};
		std::string lb{};
		bool lb_is_space = false;

		void flushLiteralBuffer()
		{
			if (lb.empty())
			{
				return;
			}

			const char* c;
			std::optional<int64_t> opt;

			if (auto tk = std::find(ld->tokens.begin(), ld->tokens.end(), lb); tk != ld->tokens.end())
			{
				lexemes.emplace_back(Lexeme{ tk->keyword });
			}
			else if (c = lb.c_str(), opt = string::toIntOpt<int64_t>(c); opt.has_value() && *c == 0)
			{
				lexemes.emplace_back(Lexeme{ Lexeme::VAL, opt.value() });
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

	std::vector<Lexeme> LangDesc::tokenise(const std::string& code) const
	{
		LexerState st{ this };

		for (auto i = code.begin(); i != code.end(); )
		{
			if (*i == '"')
			{
				st.flushLiteralBuffer();
				++i;
				st.lexemes.emplace_back(Lexeme{ Lexeme::VAL, getString(code, i), '"' });
				continue;
			}

			if (*i == '\n')
			{
				st.flushLiteralBuffer();
				st.lexemes.emplace_back(Lexeme{ Lexeme::SPACE, "\n" });
			}
			else
			{
				if (string::isSpace(*i)
					|| (*i == ';' && !semicolon_is_not_space)
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
						|| *i == '$'
						|| *i == '('
						|| *i == ')'
						|| *i == ','
						|| *i == '.'
						|| *i == ':'
						|| *i == '<'
						|| *i == '['
						|| *i == '{'
						|| *i == ';'
						)
					{
						st.flushLiteralBuffer();
					}
				}
				st.lb.push_back(*i);
				if (*i == '('
					|| *i == '>'
					)
				{
					st.flushLiteralBuffer();
				}
			}

			++i;
		}

		st.flushLiteralBuffer();

		return std::move(st.lexemes);
	}

	void LangDesc::eraseNlTerminatedComments(std::vector<Lexeme>& ls, const ConstString& prefix)
	{
		for (auto i = ls.begin(); i != ls.end(); )
		{
			if (i->token_keyword == Lexeme::LITERAL
				&& prefix.isStartOf(i->val.getString())
				)
			{
				do
				{
					i = ls.erase(i);
				} while (i != ls.end()
					&& !i->isNewLine()
					);
			}
			else
			{
				++i;
			}
		}
	}

	void LangDesc::eraseSpace(std::vector<Lexeme>& ls)
	{
		for (auto i = ls.begin(); i != ls.end(); )
		{
			if (i->token_keyword == Lexeme::SPACE)
			{
				i = ls.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	astBlock LangDesc::parse(std::vector<Lexeme> ls) const
	{
		eraseSpace(ls);
		return parseImpl(ls);
	}

	astBlock LangDesc::parseImpl(std::vector<Lexeme>& ls) const
	{
		auto b = parseNoCheck(ls);
		b.checkUnexpected();
		return b;
	}

	astBlock LangDesc::parseNoCheck(std::vector<Lexeme>& ls) const
	{
		astBlock b;
		for (auto& l : ls)
		{
			b.children.emplace_back(soup::make_unique<LexemeNode>(std::move(l)));
		}

#if DEBUG_PARSE
		std::cout << b.toString() << std::endl;
#endif

		ParserState ps;
		ps.ld = this;
		ps.b = &b;
		for (const auto& t : tokens)
		{
			if (t.parse != nullptr)
			{
#if DEBUG_PARSE
				std::cout << "Parsing " << t.keyword << std::endl;
#endif
				parseBlock(ps, t);
#if DEBUG_PARSE
				std::cout << b.toString() << std::endl;
#endif
			}
		}

		return b;
	}

	void LangDesc::parseBlock(ParserState& ps, const Token& t) const
	{
		ps.i = ps.b->children.begin();
		for (; ps.i != ps.b->children.end(); )
		{
			if ((*ps.i)->type == astNode::BLOCK)
			{
				parseBlockRecurse(ps, t, reinterpret_cast<astBlock*>(ps.i->get()));
				++ps.i;
			}
			else if ((*ps.i)->type == astNode::LEXEME)
			{
				if (t == reinterpret_cast<LexemeNode*>(ps.i->get())->lexeme.token_keyword)
				{
					t.parse(ps);
					ps.i = ps.b->children.erase(ps.i);	
					if (ps.op.type != 0xFF)
					{
						ps.i = ps.b->children.insert(ps.i, soup::make_unique<OpNode>(std::move(ps.op)));
						ps.op.type = 0xFF;
						ps.op.args.clear();
					}
					else if (!ps.op.args.empty())
					{
						std::string err = "Parser for ";
						err.append(t.keyword);
						err.append(" pushed arguments but didn't set op");
						throw ParseError(std::move(err));
					}
				}
				else
				{
					if (reinterpret_cast<LexemeNode*>(ps.i->get())->lexeme.token_keyword == Lexeme::VAL
						&& reinterpret_cast<LexemeNode*>(ps.i->get())->lexeme.val.isAstBlock()
						)
					{
						parseBlockRecurse(ps, t, &reinterpret_cast<LexemeNode*>(ps.i->get())->lexeme.val.getAstBlock());
					}
					++ps.i;
				}
			}
			else //if ((*ps.i)->type == Node::OP)
			{
				for (const auto& arg : reinterpret_cast<OpNode*>(ps.i->get())->op.args)
				{
					/*if (arg->type == Node::BLOCK)
					{
						parseBlockRecurse(ps, t, reinterpret_cast<Block*>(arg.get()));
					}
					else*/ if (arg->type == astNode::LEXEME)
					{
						if (reinterpret_cast<LexemeNode*>(arg.get())->lexeme.token_keyword == Lexeme::VAL
							&& reinterpret_cast<LexemeNode*>(arg.get())->lexeme.val.isAstBlock()
							)
						{
							parseBlockRecurse(ps, t, &reinterpret_cast<LexemeNode*>(arg.get())->lexeme.val.getAstBlock());
						}
					}
				}
				++ps.i;
			}
			/*else
			{
				++ps.i;
			}*/
		}
	}

	void LangDesc::parseBlockRecurse(ParserState& ps, const Token& t, astBlock* b) const
	{
		auto og_b = ps.b;
		auto og_i = ps.i;
		ps.b = b;
		parseBlock(ps, t);
		ps.b = og_b;
		ps.i = og_i;
	}

	const Token& LangDesc::getToken(const char* keyword) const
	{
		return *std::find(tokens.begin(), tokens.end(), keyword);
	}

	const Token& LangDesc::getToken(const Lexeme& l) const
	{
		return getToken(l.token_keyword);
	}

	FormattedText LangDesc::highlightSyntax(const std::string& code) const
	{
		return highlightSyntax(tokenise(code));
	}

	FormattedText LangDesc::highlightSyntax(const std::vector<Lexeme>& ls) const
	{
		FormattedText ft;
		std::vector<FormattedText::Span> line{};
		for (const auto& l : ls)
		{
			if (l.isNewLine())
			{
				ft.lines.emplace_back(std::move(line));
				line.clear();
			}
			else
			{
				if (l.isBuiltin())
				{
					if (l.token_keyword == Lexeme::VAL)
					{
						if (l.val.isInt())
						{
							line.emplace_back(FormattedText::Span{ l.val.toString(), Rgb::BLUE });
						}
						else
						{
							line.emplace_back(FormattedText::Span{ l.getSourceString(), Rgb::YELLOW });
						}
					}
					else
					{
						line.emplace_back(FormattedText::Span{ l.val.toString(), Rgb::WHITE });
					}
				}
				else
				{
					line.emplace_back(FormattedText::Span{ l.token_keyword, getToken(l).colour });
				}
			}
		}
		if (!line.empty())
		{
			ft.lines.emplace_back(std::move(line));
		}
		return ft;
	}
}
