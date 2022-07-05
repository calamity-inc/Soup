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
			std::string err = reinterpret_cast<ast::LexemeNode*>(i->get())->lexeme.token_keyword;
			err.append(" expected lefthand value, found ");
			err.append(node->toString());
			throw ParseError(std::move(err));
		}
		pushArgNode(std::move(node));
#if ENSURE_SANITY
		ensureValidIterator();
#endif
	}

	void ParserState::consumeRighthandValue()
	{
		auto node = popRighthand();
		if (!node->isValue())
		{
			std::string err = reinterpret_cast<ast::LexemeNode*>(i->get())->lexeme.token_keyword;
			err.append(" expected righthand value, found ");
			err.append(node->toString());
			throw ParseError(std::move(err));
		}
		pushArgNode(std::move(node));
#if ENSURE_SANITY
		ensureValidIterator();
#endif
	}

	void ParserState::pushArg(Mixed&& val)
	{
		pushArgNode(soup::make_unique<ast::LexemeNode>(Lexeme{ Lexeme::VAL, std::move(val) }));
	}

	void ParserState::pushArgNode(UniquePtr<ast::Node>&& node)
	{
		op.args.emplace_back(std::move(node));
	}

	void ParserState::setArgs(std::vector<UniquePtr<ast::Node>>&& args)
	{
		op.args = std::move(args);
	}

	void ParserState::pushLefthand(Mixed&& val)
	{
		pushLefthand(Lexeme{ Lexeme::VAL, std::move(val) });
	}

	void ParserState::pushLefthand(Lexeme&& l)
	{
		pushLefthandNode(soup::make_unique<ast::LexemeNode>(std::move(l)));
	}

	void ParserState::pushLefthandNode(UniquePtr<ast::Node>&& node)
	{
		i = b->children.insert(i, std::move(node));
		++i;
	}

	UniquePtr<ast::Node> ParserState::popLefthand()
	{
		if (i == b->children.begin())
		{
			std::string err = reinterpret_cast<ast::LexemeNode*>(i->get())->lexeme.token_keyword;
			err.append(" expected lefthand, found start of block");
			throw ParseError(std::move(err));
		}
		--i;
		UniquePtr<ast::Node> ret = std::move(*i);
		i = b->children.erase(i);
#if ENSURE_SANITY
		ensureValidIterator();
#endif
		return ret;
	}

	ast::Node* ParserState::peekRighthand() const
	{
		checkRighthand();
		return (i + 1)->get();
	}

	UniquePtr<ast::Node> ParserState::popRighthand()
	{
		checkRighthand();
		++i;
		UniquePtr<ast::Node> ret = std::move(*i);
		i = b->children.erase(i);
		--i;
#if ENSURE_SANITY
		ensureValidIterator();
#endif
		return ret;
	}

	UniquePtr<ast::Block> ParserState::collapseRighthandBlock(const char* end_token)
	{
		auto b = soup::make_unique<ast::Block>();
		for (UniquePtr<ast::Node> node;
			node = popRighthand(), node->type != ast::Node::LEXEME || reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.token_keyword != end_token;
			)
		{
			b->children.emplace_back(std::move(node));
		}
		return b;
	}

	const Token& ParserState::getToken() const
	{
		return ld->getToken(reinterpret_cast<ast::LexemeNode*>(i->get())->lexeme);
	}

	void ParserState::checkRighthand() const
	{
		if ((i + 1) == b->children.end())
		{
			std::string err = reinterpret_cast<ast::LexemeNode*>(i->get())->lexeme.token_keyword;
			err.append(" expected righthand, found end of block");
			throw ParseError(std::move(err));
		}
	}

	void ParserState::ensureValidIterator() const
	{
		if ((*i)->type != ast::Node::LEXEME
			|| reinterpret_cast<ast::LexemeNode*>(i->get())->lexeme.token_keyword == Lexeme::VAL
			)
		{
			throw 0;
		}
	}
}
