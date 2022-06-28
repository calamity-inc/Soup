#pragma once

#include "fwd.hpp"

#include <string>

#include "Lexeme.hpp"
#include "Op.hpp"
#include "TreeNode.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct ParseTreeNode : public TreeNode
	{
		enum Type : uint8_t
		{
			BLOCK = 0,
			LEXEME,
			OP,
		};

		const Type type;

		ParseTreeNode(Type type)
			: type(type)
		{
		}

		[[nodiscard]] std::string toString(const std::string& prefix = {}) const;

		void compile(CompilerState& st) const;
	};

	struct Block : public ParseTreeNode
	{
		std::vector<UniquePtr<ParseTreeNode>> children{};

		Block(std::vector<UniquePtr<ParseTreeNode>>&& children = {})
			: ParseTreeNode(BLOCK), children(std::move(children))
		{
		}

		void checkUnexpected() const;

		[[nodiscard]] std::string toString(std::string prefix = {}) const;

		void compile(Writer& w) const;
		void compile(CompilerState& st) const;
	};

	struct LexemeNode : public ParseTreeNode
	{
		Lexeme lexeme;

		LexemeNode(Lexeme lexeme)
			: ParseTreeNode(LEXEME), lexeme(std::move(lexeme))
		{
		}

		[[nodiscard]] std::string toString(const std::string& prefix = {}) const;

		void compile(CompilerState& st) const;
	};

	struct OpNode : public ParseTreeNode
	{
		Op op;

		OpNode(Op&& op)
			: ParseTreeNode(OP), op(std::move(op))
		{
		}

		[[nodiscard]] std::string toString(std::string prefix = {}) const;

		void compile(CompilerState& st) const;
	};
}
