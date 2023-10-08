#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdDelete : public cbCmd
	{
		[[nodiscard]] bool checkTriggers(cbParser& p) const noexcept final
		{
			return p.checkTrigger("delete");
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			auto s = p.getArgNumeric();
			if (int64_t i; string::toInt<int64_t, string::TI_FULL>(s).consume(i))
			{
				cbResult res(CB_RES_DELETE);
				res.extra = static_cast<int64_t>(i);
				return res;
			}
			return "Delete ...?";
		}
	};
}
