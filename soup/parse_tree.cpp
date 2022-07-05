#include "parse_tree.hpp"

#include "BuiltinOp.hpp"
#include "Lexeme.hpp"
#include "ParseError.hpp"
#include "StringWriter.hpp"

namespace soup::ast
{
	std::string Node::toString(const std::string& prefix) const
	{
		if (type == Node::BLOCK)
		{
			return reinterpret_cast<const Block*>(this)->toString(prefix);
		}
		else if (type == Node::LEXEME)
		{
			return reinterpret_cast<const LexemeNode*>(this)->toString(prefix);
		}
		else //if (type == ParseTreeNode::OP)
		{
			return reinterpret_cast<const OpNode*>(this)->toString(prefix);
		}
	}

	bool Node::isValue() const noexcept
	{
		switch (type)
		{
		case Node::LEXEME:
			return reinterpret_cast<const LexemeNode*>(this)->lexeme.token_keyword == Lexeme::LITERAL // variable name
				|| reinterpret_cast<const LexemeNode*>(this)->lexeme.token_keyword == Lexeme::VAL // rvalue
				;

		case Node::OP: // result of an expression
			return true;

		default:
			break;
		}
		// otherwise, probably an oopsie
		return false;
	}

	void Node::compile(Writer& w) const
	{
		if (type == Node::BLOCK)
		{
			reinterpret_cast<const Block*>(this)->compile(w);
		}
		else if (type == Node::LEXEME)
		{
			reinterpret_cast<const LexemeNode*>(this)->compile(w);
		}
		else //if (type == ParseTreeNode::OP)
		{
			reinterpret_cast<const OpNode*>(this)->compile(w);
		}
	}

	// Block

	void Block::checkUnexpected() const
	{
		for (const auto& child : children)
		{
			if (child->type == Node::BLOCK)
			{
				reinterpret_cast<const Block*>(child.get())->checkUnexpected();
			}
			else if (child->type == Node::LEXEME)
			{
				std::string err = "Unexpected ";
				err.append(child->toString());
				throw ParseError(std::move(err));
			}
		}
	}

	std::string Block::toString(std::string prefix) const
	{
		std::string str = "block";
		for (const auto& child : param_literals)
		{
			str.append(" [");
			str.append(child->toString());
			str.push_back(']');
		}
		prefix.push_back('\t');
		for (const auto& child : children)
		{
			str.push_back('\n');
			str.append(prefix);
			str.append(child->toString(prefix));
		}
		return str;
	}

	void Block::compile(Writer& w) const
	{
		if (size_t num_params = param_literals.size())
		{
			for (const auto& param : param_literals)
			{
				param->compile(w);
			}
			uint8_t b = OP_POP_ARGS;
			w.u8(b);
			w.u64_dyn(num_params);
		}
		for (const auto& child : children)
		{
			child->compile(w);
		}
	}

	// LexemeNode

	std::string LexemeNode::toString(const std::string& prefix) const
	{
		return lexeme.toString(prefix);
	}

	void LexemeNode::compile(Writer& w) const
	{
		if (lexeme.token_keyword == Lexeme::VAL)
		{
			if (lexeme.val.isInt())
			{
				uint8_t b = OP_PUSH_INT;
				w.u8(b);
				w.i64_dyn(lexeme.val.getInt());
				return;
			}
			if (lexeme.val.isUInt())
			{
				uint8_t b = OP_PUSH_UINT;
				w.u8(b);
				w.u64_dyn(lexeme.val.getUInt());
				return;
			}
			if (lexeme.val.isString())
			{
				uint8_t b = OP_PUSH_STR;
				w.u8(b);
				w.str_lp_u64_dyn(lexeme.val.getString());
				return;
			}
			if (lexeme.val.isAstBlock())
			{
				StringWriter sw;
				lexeme.val.getAstBlock().compile(sw);

				uint8_t b = OP_PUSH_FUN;
				w.u8(b);
				w.str_lp_u64_dyn(std::move(sw.str));
				return;
			}
		}
		else if (lexeme.token_keyword == Lexeme::LITERAL)
		{
			uint8_t b = OP_PUSH_VAR;
			w.u8(b);
			w.str_lp_u64_dyn(lexeme.val.getString());
			return;
		}
		std::string err = "Non-compilable lexeme in parse tree at compile time: ";
		err.append(toString());
		throw ParseError(std::move(err));
	}

	// AstOpNode

	std::string OpNode::toString(std::string prefix) const
	{
		std::string str{};
		str.append("op ");
		str.append(std::to_string(op.type));
		if (!op.args.empty())
		{
			prefix.push_back('\t');
			for (const auto& arg : op.args)
			{
				str.push_back('\n');
				str.append(prefix);
				str.append(arg->toString(prefix));
			}
		}
		return str;
	}

	void OpNode::compile(Writer& w) const
	{
		for (auto i = op.args.rbegin(); i != op.args.rend(); ++i)
		{
			(*i)->compile(w);
		}
		uint8_t b = op.type;
		w.u8(b);
	}
}
