#include "MathExpr.hpp"

#include "irModule.hpp"
#include "irVm.hpp"
#include "laMathFrontend.hpp"

namespace soup
{
	Optional<int64_t> MathExpr::evaluate(const std::string& str)
	{
		laMathFrontend f;
		auto m = f.parse(str);
		auto memory = m.getContiguousMemory();
		irVm vm(memory);
		auto ret = vm.execute(m, m.func_exports.at(0));
		if (!ret.empty())
		{
			return ret.at(0).value.i64;
		}
		return std::nullopt;
	}
}
