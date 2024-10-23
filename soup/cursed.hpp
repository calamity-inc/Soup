#pragma once

#include "base.hpp"

NAMESPACE_SOUP
{
	template <typename Desired, typename Actual>
	[[nodiscard]] Desired ub_cast(Actual a)
	{
		union {
			Actual a;
			Desired d;
		} u{ a = a };
		SOUP_MOVE_RETURN(u.d);
	}

	template <typename Desired, typename Actual>
	[[nodiscard]] Desired ub_pointer_cast(Actual a)
	{
		static_assert(sizeof(Desired) == sizeof(Actual));
		return ub_cast<Desired, Actual>(a);
	}
}
