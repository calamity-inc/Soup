#pragma once

#include "base.hpp"
#include "fwd.hpp"

namespace soup
{
	struct memVft
	{
		void** vft;

		[[nodiscard]] size_t getNumFunctions() const noexcept;
		[[nodiscard]] void* getFunction(size_t i) const noexcept;

#if SOUP_WINDOWS // Because Module is not cross-platfom *yet*
		[[nodiscard]] bool hasRtti() const noexcept;
#endif
		[[nodiscard]] rtti::object* getRtti() const noexcept;
	};
}
