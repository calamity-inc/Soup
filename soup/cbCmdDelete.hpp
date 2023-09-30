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
			long i;
			// Not ideal because std::stol may throw.
#if SOUP_EXCEPTIONS
			try
#endif
			{
				i = std::stol(s);
			}
#if SOUP_EXCEPTIONS
			catch (...)
			{
				return "Delete ...?";
			}
#endif
			cbResult res(CB_RES_DELETE);
			res.delete_args.num = i;
			return res;
		}
	};
}
