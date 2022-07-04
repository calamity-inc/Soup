#pragma once

#include "fwd.hpp"

#include <vector>

#include "Op.hpp"

namespace soup
{
	class ParserState
	{
	public:
		const LangDesc* ld;
		ast::Block* b;
		std::vector<UniquePtr<ast::Node>>::iterator i;
		Op op;

		void setOp(int type);
		void consumeLefthandValue();
		void consumeRighthandValue();

		void pushArg(Mixed&& val);
		void pushArgNode(UniquePtr<ast::Node>&& node);
		void setArgs(std::vector<UniquePtr<ast::Node>>&& args);

		void pushLefthand(Mixed&& val);
		void pushLefthand(Lexeme&& l);
		void pushLefthandNode(UniquePtr<ast::Node>&& node);
		UniquePtr<ast::Node> popLefthand();
		[[nodiscard]] ast::Node* peekRighthand() const;
		UniquePtr<ast::Node> popRighthand();

		[[nodiscard]] const Token& getToken() const;

	private:
		void checkRighthand() const;
		void ensureValidIterator() const;
	};
}
