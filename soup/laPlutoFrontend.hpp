#pragma once

#include "laFrontend.hpp"

#include "fwd.hpp" // LexemeParser
#include <stack>

NAMESPACE_SOUP
{
	// Can convert a small subset of Pluto (https://pluto-lang.org) code into Soup IR.
	class laPlutoFrontend : public laFrontend
	{
	public:
		[[nodiscard]] irModule parse(const std::string& program) final;
	protected:
		[[nodiscard]] std::vector<UniquePtr<irExpression>> statlist(LexemeParser& lp, irModule& m, irFunction& fn);
		[[nodiscard]] UniquePtr<irExpression> expr(LexemeParser& lp, irModule& m, irFunction& fn, uint8_t limit = 0);
		[[nodiscard]] UniquePtr<irExpression> simpleexp(LexemeParser& lp, irModule& m, irFunction& fn);
		[[nodiscard]] UniquePtr<irExpression> exprstat(LexemeParser& lp, irModule& m, irFunction& fn);
		[[nodiscard]] UniquePtr<irExpression> suffixedexp(LexemeParser& lp, irModule& m, irFunction& fn);
		void funcargs(LexemeParser& lp, irModule& m, irFunction& fn, irExpression& insn);

		std::stack<std::vector<std::string>> locals{};

		[[nodiscard]] static UniquePtr<irExpression> oneret(irModule& m, UniquePtr<irExpression>&& insn);
	};
}
