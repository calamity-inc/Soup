#include "ParserState.hpp"

#include <string>

#include "LangDesc.hpp"
#include "Lexeme.hpp"
#include "ParseError.hpp"
#include "parse_tree.hpp"
#include "UniquePtr.hpp"

#define ENSURE_SANITY false

namespace soup
{
	void ParserState::setOp(int type)
	{
		op.type = type;
	}

	void ParserState::consumeLefthandValue()
	{
		auto node = popLefthand();
		if (!node->isValue())
		{
			std::string err = reinterpret_cast<LexemeNode*>(i->get())->lexeme.token_keyword;
			err.append(" expected lefthand value, found ");
			err.append(node->toString());
			throw ParseError(std::move(err));
		}
		op.args.emplace_back(std::move(node));
#if ENSURE_SANITY
		ensureValidIterator();
#endif
	}

	void ParserState::consumeRighthandValue()
	{
		auto node = popRighthand();
		if (!node->isValue())
		{
			std::string err = reinterpret_cast<LexemeNode*>(i->get())->lexeme.token_keyword;
			err.append(" expected righthand value, found ");
			err.append(node->toString());
			throw ParseError(std::move(err));
		}
		op.args.emplace_back(std::move(node));
#if ENSURE_SANITY
		ensureValidIterator();
#endif
	}

	void ParserState::pushArg(Mixed&& val)
	{
		op.args.emplace_back(soup::make_unique<LexemeNode>(Lexeme{ Lexeme::VAL, std::move(val) }));
	}

	void ParserState::setArgs(std::vector<UniquePtr<ParseTreeNode>>&& args)
	{
		op.args = std::move(args);
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

	UniquePtr<ParseTreeNode> ParserState::popLefthand()
	{
		if (i == b->children.begin())
		{
			std::string err = reinterpret_cast<LexemeNode*>(i->get())->lexeme.token_keyword;
			err.append(" expected lefthand, found start of block");
			throw ParseError(std::move(err));
		}
		--i;
		UniquePtr<ParseTreeNode> ret = std::move(*i);
		i = b->children.erase(i);
#if ENSURE_SANITY
		ensureValidIterator();
#endif
		return ret;
	}

	ParseTreeNode* ParserState::peekRighthand() const
	{
		checkRighthand();
		return (i + 1)->get();
	}

	UniquePtr<ParseTreeNode> ParserState::popRighthand()
	{
		checkRighthand();
		++i;
		UniquePtr<ParseTreeNode> ret = std::move(*i);
		i = b->children.erase(i);
		--i;
#if ENSURE_SANITY
		ensureValidIterator();
#endif
		return ret;
	}

	const Token& ParserState::getToken() const
	{
		return ld->getToken(reinterpret_cast<LexemeNode*>(i->get())->lexeme);
	}

	void ParserState::checkRighthand() const
	{
		if ((i + 1) == b->children.end())
		{
			std::string err = reinterpret_cast<LexemeNode*>(i->get())->lexeme.token_keyword;
			err.append(" expected righthand, found end of block");
			throw ParseError(std::move(err));
		}
	}

	void ParserState::ensureValidIterator() const
	{
		if ((*i)->type != ParseTreeNode::LEXEME
			|| reinterpret_cast<LexemeNode*>(i->get())->lexeme.token_keyword == Lexeme::VAL
			)
		{
			throw 0;
		}
	}
}
