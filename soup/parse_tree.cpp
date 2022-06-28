#include "parse_tree.hpp"

#include "BuiltinOp.hpp"
#include "CompilerState.hpp"
#include "Lexeme.hpp"
#include "ParseError.hpp"
#include "StringWriter.hpp"

namespace soup
{
	std::string ParseTreeNode::toString(const std::string& prefix) const
	{
		if (type == ParseTreeNode::BLOCK)
		{
			return reinterpret_cast<const Block*>(this)->toString(prefix);
		}
		else if (type == ParseTreeNode::LEXEME)
		{
			return reinterpret_cast<const LexemeNode*>(this)->toString(prefix);
		}
		else //if (type == ParseTreeNode::OP)
		{
			return reinterpret_cast<const OpNode*>(this)->toString(prefix);
		}
	}

	void ParseTreeNode::compile(CompilerState& st) const
	{
		if (type == ParseTreeNode::BLOCK)
		{
			reinterpret_cast<const Block*>(this)->compile(st);
		}
		else if (type == ParseTreeNode::LEXEME)
		{
			reinterpret_cast<const LexemeNode*>(this)->compile(st);
		}
		else //if (type == ParseTreeNode::OP)
		{
			reinterpret_cast<const OpNode*>(this)->compile(st);
		}
	}

	// Block

	void Block::checkUnexpected() const
	{
		for (const auto& child : children)
		{
			if (child->type == ParseTreeNode::BLOCK)
			{
				reinterpret_cast<const Block*>(child.get())->checkUnexpected();
			}
			else if (child->type == ParseTreeNode::LEXEME)
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
		CompilerState st{ &w };
		compile(st);
	}

	void Block::compile(CompilerState& st) const
	{
		for (const auto& child : children)
		{
			child->compile(st);
		}
	}

	// LexemeNode

	std::string LexemeNode::toString(const std::string& prefix) const
	{
		return lexeme.toString(prefix);
	}

	void LexemeNode::compile(CompilerState& st) const
	{
		if (lexeme.token_keyword == Lexeme::VAL)
		{
			if (lexeme.val.isInt())
			{
				uint8_t b = OP_PUSH_INT;
				st.w->u8(b);
				st.w->i64_dyn(lexeme.val.getInt());
				return;
			}
			if (lexeme.val.isString())
			{
				uint8_t b = OP_PUSH_STR;
				st.w->u8(b);
				st.w->str_lp_u64_dyn(lexeme.val.getString());
				return;
			}
			if (lexeme.val.isBlock())
			{
				auto og_w = st.w;
				StringWriter w;
				st.w = &w;
				lexeme.val.getBlock().compile(st);
				st.w = og_w;

				uint8_t b = OP_PUSH_FUN;
				st.w->u8(b);
				st.w->str_lp_u64_dyn(std::move(w.str));
				return;
			}
		}
		else if (lexeme.token_keyword == Lexeme::LITERAL)
		{
			uint8_t b = OP_PUSH_VAR;
			st.w->u8(b);
			auto idx = st.getVarIndex(lexeme.val.getString());
			if (idx > 0xFF)
			{
				throw ParseError("Too many variables");
			}
			b = idx;
			st.w->u8(b);
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

	void OpNode::compile(CompilerState& st) const
	{
		for (auto i = op.args.rbegin(); i != op.args.rend(); ++i)
		{
			(*i)->compile(st);
		}
		uint8_t b = op.type;
		st.w->u8(b);
	}
}
