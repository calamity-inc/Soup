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
		Block* b;
		std::vector<UniquePtr<ParseTreeNode>>::iterator i;
		Op op;

		void setOp(int type);
		void consumeLefthandValue();
		void consumeRighthandValue();

		void pushArg(Mixed&& val);

		void pushLefthand(UniquePtr<ParseTreeNode>&& node);
		void pushLefthand(Lexeme&& l);
		[[nodiscard]] ParseTreeNode* peekRighthand();
		UniquePtr<ParseTreeNode> popRighthand();

		[[nodiscard]] const Token& getToken() const;

	private:
		[[nodiscard]] bool isValue() const noexcept;
	};
}
