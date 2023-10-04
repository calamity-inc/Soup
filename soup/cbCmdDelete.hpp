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
			long long i;
			// Not ideal because std::stoll may throw.
#if SOUP_EXCEPTIONS
			try
#endif
			{
				i = std::stoll(s);
			}
#if SOUP_EXCEPTIONS
			catch (...)
			{
				return "Delete ...?";
			}
#endif
			cbResult res(CB_RES_DELETE);
			res.extra = static_cast<int64_t>(i);
			return res;
		}
	};
}
