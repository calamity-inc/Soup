#pragma once

namespace soup
{
	struct optimised
	{
		template <typename T>
		[[nodiscard]] constexpr static T trinary(bool cond, T a, T b)
		{
			return (cond * a) + ((!cond) * b);
		}

		template <typename T>
		[[nodiscard]] constexpr static T min(T a, T b)
		{
			return trinary(a < b, a, b);
		}

		template <typename T>
		[[nodiscard]] constexpr static T max(T a, T b)
		{
			return trinary(b < a, a, b);
		}
	};
}
