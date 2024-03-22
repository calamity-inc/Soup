#pragma once

#include "laFrontend.hpp"

#include "fwd.hpp" // LexemeParser
#include <stack>

namespace soup
{
	// Can convert a small subset of Pluto (https://pluto-lang.org) code into Soup IR.
	class laPlutoFrontend : public laFrontend
	{
	public:
		[[nodiscard]] irModule parse(const std::string& program) final;
	protected:
		void statlist(LexemeParser& lp, irModule& m, irModule::FuncExport& fn);
		[[nodiscard]] UniquePtr<irExpression> expr(LexemeParser& lp, uint8_t limit = 0);
		[[nodiscard]] UniquePtr<irExpression> simpleexp(LexemeParser& lp);
		[[nodiscard]] UniquePtr<irExpression> exprstat(LexemeParser& lp);
		[[nodiscard]] UniquePtr<irExpression> suffixedexp(LexemeParser& lp);

		std::stack<std::vector<std::string>> locals{};
		irModule* m;
	};
}
