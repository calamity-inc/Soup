#pragma once

#include "fwd.hpp"

#include <string>

#include "Lexeme.hpp"
#include "Op.hpp"
#include "TreeNode.hpp"
#include "UniquePtr.hpp"

namespace soup::ast
{
	struct Node : public TreeNode
	{
		enum Type : uint8_t
		{
			BLOCK = 0,
			LEXEME,
			OP,
		};

		const Type type;

		Node(Type type) noexcept
			: type(type)
		{
		}

		[[nodiscard]] bool isValue() const noexcept;

		[[nodiscard]] std::string toString(const std::string& prefix = {}) const;

		void compile(Writer& w) const;
	};

	struct Block : public Node
	{
		std::vector<UniquePtr<Node>> children{};
		std::vector<UniquePtr<Node>> param_literals;

		Block(std::vector<UniquePtr<Node>>&& children = {})
			: Node(BLOCK), children(std::move(children))
		{
		}

		void checkUnexpected() const;

		[[nodiscard]] std::string toString(std::string prefix = {}) const;

		void compile(Writer& w) const;
	};

	struct LexemeNode : public Node
	{
		Lexeme lexeme;

		LexemeNode(Lexeme lexeme)
			: Node(LEXEME), lexeme(std::move(lexeme))
		{
		}

		[[nodiscard]] std::string toString(const std::string& prefix = {}) const;

		void compile(Writer& w) const;
	};

	struct OpNode : public Node
	{
		Op op;

		OpNode(Op&& op)
			: Node(OP), op(std::move(op))
		{
		}

		[[nodiscard]] std::string toString(std::string prefix = {}) const;

		void compile(Writer& w) const;
	};
}
