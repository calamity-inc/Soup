#pragma once

#include "fwd.hpp"

namespace soup
{
	struct aglStat
	{
		virtual ~aglStat() = default;

		virtual void transpile(aglTranspiler& t) const = 0;
	};
}
