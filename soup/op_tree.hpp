#pragma once

#include "Lexer.hpp"
#include "TreeNode.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct OpTreeNode : public TreeNode
	{
		const bool is_block;

		OpTreeNode(bool is_block)
			: is_block(is_block)
		{
		}
	};

	struct OpNode : public OpTreeNode
	{
		Op op;

		OpNode(Op&& op)
			: OpTreeNode(false), op(std::move(op))
		{
		}

		[[nodiscard]] std::string toString(const Lexer& lexer) const
		{
			std::string str{};
			str.append(lexer.getName(op.id));
			if (!op.args.empty())
			{
				str.append(": ");
				for (const auto& arg : op.args)
				{
					str.push_back('[');
					str.append(lexer.getName(arg));
					str.push_back(']');
				}
			}
			return str;
		}
	};

	struct Block : public OpTreeNode
	{
		std::vector<UniquePtr<OpTreeNode>> children{};

		Block()
			: OpTreeNode(true)
		{
		}

		void addOp(Op&& op)
		{
			children.emplace_back(soup::make_unique<OpNode>(std::move(op)));
		}

		[[nodiscard]] std::string toString(const Lexer& lexer, const std::string& prefix = {}) const
		{
			std::string str{};
			for (const auto& child : children)
			{
				str.append(prefix);
				if (child->is_block)
				{
					str.append("<block>\n");
					std::string prefix_ = prefix;
					prefix_.push_back('\t');
					str.append(reinterpret_cast<Block*>(child.get())->toString(lexer, prefix_));
				}
				else
				{
					str.append(reinterpret_cast<OpNode*>(child.get())->toString(lexer));
					str.push_back('\n');
				}
			}
			return str;
		}

		void flatten(std::vector<Op>& ops) const
		{
			for (const auto& child : children)
			{
				if (child->is_block)
				{
					reinterpret_cast<Block*>(child.get())->flatten(ops);
				}
				else
				{
					ops.emplace_back(std::move(reinterpret_cast<OpNode*>(child.get())->op));
				}
			}
		}
	};
}
