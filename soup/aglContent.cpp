#include "aglContent.hpp"

#include "aglTranspiler.hpp"
#include "LangDesc.hpp"
#include "ParserState.hpp"
#include "parse_tree.hpp"
#include "PhpState.hpp"

#include "aglStatEcho.hpp"

namespace soup
{
	[[nodiscard]] static auto opArgToAglRepresentation(const UniquePtr<astNode>& arg)
	{
		if (arg->type != astNode::LEXEME)
		{
			throw 0;
		}
		auto lexemenode = reinterpret_cast<const LexemeNode*>(arg.get());
		return lexemenode->lexeme.getSourceString();
	}

	enum aglOpType
	{
		OP_ECHO,
	};

	aglContent aglContent::fromSource(const std::string& code)
	{
		LangDesc ld;
		ld.addToken("echo", [](ParserState& ps)
		{
			ps.setOp(OP_ECHO);
			ps.consumeRighthandValue();
		});

		auto ls = ld.tokenise(code);
		auto ast = ld.parse(ls);
		aglContent cont;
		for (const auto& node : ast.children)
		{
			if (node->type != astNode::OP)
			{
				throw 0;
			}
			auto opnode = reinterpret_cast<const OpNode*>(node.get());
			switch (opnode->op.type)
			{
			case OP_ECHO:
				cont.statements.emplace_back(soup::make_unique<aglStatEcho>(opArgToAglRepresentation(opnode->op.args.at(0))));
				break;
			}
		}
		return cont;
	}

	void aglContent::transpile(aglTranspiler& t) const
	{
		if (t.target == AGL_PHP)
		{
			t.body.append("<?php\n");
		}
		if (t.target == AGL_CPP)
		{
			t.body.append("int main()\n{\n");
			++t.indentation;
		}
		for (const auto& stat : statements)
		{
			stat->transpile(t);
		}
		if (t.target == AGL_CPP)
		{
			--t.indentation;
			t.body.append("}\n");
		}
	}

	std::string aglContent::evaluate() const
	{
		aglTranspiler t(AGL_PHP);
		transpile(t);

		PhpState ps;
		return ps.evaluate(t.getOutput());
	}
}
