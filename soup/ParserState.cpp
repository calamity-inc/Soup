#include "ParserState.hpp"

#include <string>

#include "LangDesc.hpp"
#include "Lexeme.hpp"
#include "ParseError.hpp"
#include "parse_tree.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	void ParserState::setOp(int type)
	{
		op.type = type;
	}

	void ParserState::consumeLefthandValue()
	{
		if (i == b->children.begin())
		{
			std::string err = reinterpret_cast<LexemeNode*>(i->get())->lexeme.token_keyword;
			err.append(" expected lefthand value, found start of block");
			throw ParseError(std::move(err));
		}
		--i;
		if (!isValue())
		{
			std::string err = reinterpret_cast<LexemeNode*>((i + 1)->get())->lexeme.token_keyword;
			err.append(" expected lefthand value, found ");
			err.append((*i)->toString());
			throw ParseError(std::move(err));
		}
		op.args.emplace_back(std::move(*i));
		i = b->children.erase(i);
	}

	void ParserState::consumeRighthandValue()
	{
		++i;
		if (i == b->children.end())
		{
			--i;
			std::string err = reinterpret_cast<LexemeNode*>(i->get())->lexeme.token_keyword;
			err.append(" expected righthand value, found end of block");
			throw ParseError(std::move(err));
		}
		if (!isValue())
		{
			std::string err = reinterpret_cast<LexemeNode*>((i - 1)->get())->lexeme.token_keyword;
			err.append(" expected righthand value, found ");
			err.append((*i)->toString());
			throw ParseError(std::move(err));
		}
		op.args.emplace_back(std::move(*i));
		i = b->children.erase(i);
		--i;
	}

	void ParserState::pushArg(Mixed&& val)
	{
		op.args.emplace_back(soup::make_unique<LexemeNode>(Lexeme{ Lexeme::VAL, std::move(val) }));
	}

	void ParserState::pushLefthand(UniquePtr<ParseTreeNode>&& node)
	{
		i = b->children.insert(i, std::move(node));
		++i;
	}

	void ParserState::pushLefthand(Lexeme&& l)
	{
		pushLefthand(soup::make_unique<LexemeNode>(std::move(l)));
	}

	ParseTreeNode* ParserState::peekRighthand()
	{
		++i;
		if (i == b->children.end())
		{
			--i;
			std::string err = reinterpret_cast<LexemeNode*>(i->get())->lexeme.token_keyword;
			err.append(" expected righthand token, found end of block");
			throw ParseError(std::move(err));
		}
		ParseTreeNode* ret = i->get();
		--i;
		return ret;
	}

	UniquePtr<ParseTreeNode> ParserState::popRighthand()
	{
		++i;
		if (i == b->children.end())
		{
			--i;
			std::string err = reinterpret_cast<LexemeNode*>(i->get())->lexeme.token_keyword;
			err.append(" expected righthand token, found end of block");
			throw ParseError(std::move(err));
		}
		UniquePtr<ParseTreeNode> ret = std::move(*i);
		i = b->children.erase(i);
		--i;
		return ret;
	}

	const Token& ParserState::getToken() const
	{
		return ld->getToken(reinterpret_cast<LexemeNode*>(i->get())->lexeme);
	}

	bool ParserState::isValue() const noexcept
	{
		switch ((*i)->type)
		{
		case ParseTreeNode::LEXEME:
			return reinterpret_cast<LexemeNode*>(i->get())->lexeme.token_keyword == Lexeme::LITERAL // variable name
				|| reinterpret_cast<LexemeNode*>(i->get())->lexeme.token_keyword == Lexeme::VAL // rvalue
				;

		case ParseTreeNode::OP: // result of an expression
			return true;

		default:
			break;
		}
		// otherwise, probably an oopsie
		return false;
	}
}
