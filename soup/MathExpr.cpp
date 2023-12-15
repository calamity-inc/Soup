#include "MathExpr.hpp"

#include "LangDesc.hpp"
#include "LangVm.hpp"
#include "ParserState.hpp"
#include "parse_tree.hpp" // astBlock
#include "StringReader.hpp"
#include "StringWriter.hpp"

namespace soup
{
	enum
	{
		OP_ADD,
		OP_SUB,
		OP_MUL,
		OP_DIV,
	};

	int64_t MathExpr::evaluate(const std::string& str)
	{
		LangDesc ld;

		ld.addToken("*", [](soup::ParserState& ps)
			{
			ps.setOp(OP_MUL);
			ps.consumeLefthandValue();
			ps.consumeRighthandValue();
		});
		ld.addTokenWithSamePrecedenceAsPreviousToken("/", [](soup::ParserState& ps)
		{
			ps.setOp(OP_DIV);
			ps.consumeLefthandValue();
			ps.consumeRighthandValue();
		});

		ld.addToken("+", [](soup::ParserState& ps)
		{
			ps.setOp(OP_ADD);
			ps.consumeLefthandValue();
			ps.consumeRighthandValue();
		});
		ld.addTokenWithSamePrecedenceAsPreviousToken("-", [](soup::ParserState& ps)
		{
			ps.setOp(OP_SUB);
			ps.consumeLefthandValue();
			ps.consumeRighthandValue();
		});

		astBlock root = ld.parse(ld.tokenise(str));

		StringWriter w;
		root.compile(w);

		StringReader r{ std::move(w.data) };	
		LangVm vm{ r };
		for (uint8_t op; vm.getNextOp(op); )
		{
			switch (op)
			{
			case OP_ADD:
				vm.push(vm.pop()->getInt() + vm.pop()->getInt());
				break;

			case OP_SUB:
				vm.push(vm.pop()->getInt() - vm.pop()->getInt());
				break;

			case OP_MUL:
				vm.push(vm.pop()->getInt() * vm.pop()->getInt());
				break;

			case OP_DIV:
				vm.push(vm.pop()->getInt() / vm.pop()->getInt());
				break;
			}
		}
		return vm.pop()->getInt();
	}
}
