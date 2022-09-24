#pragma once

#include "base.hpp"
#include "fwd.hpp"

namespace soup
{
	struct memObject
	{
		const void* inst;

#if SOUP_WINDOWS // Because Module is not cross-platfom *yet*
		[[nodiscard]] bool hasVft() const noexcept;
#endif
		[[nodiscard]] memVft getVft() const noexcept;
	};
}
