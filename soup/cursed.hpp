#pragma once

namespace soup
{
	template <typename Desired, typename Actual>
	[[nodiscard]] Desired ub_pointer_cast(Actual a)
	{
		static_assert(sizeof(Desired) == sizeof(Actual));
		union {
			Actual a;
			Desired d;
		} u{ a = a };
		return u.d;
	}

	template <typename Desired, typename Actual>
	[[nodiscard]] Desired ub_cast(Actual a)
	{
		union {
			Actual a;
			Desired d;
		} u{ a = a };
		return u.d;
	}
}
