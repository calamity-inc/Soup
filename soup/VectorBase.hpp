#pragma once

#include <cfloat>
#include <cstdint>

#include "math.hpp"

#undef min
#undef max

NAMESPACE_SOUP
{
	template <typename T>
	[[nodiscard]] constexpr T pow2(T i) noexcept
	{
		return i * i;
	}

	template <uint8_t axes, typename T>
	struct VectorBase
	{
		[[nodiscard]] static constexpr uint8_t getAxes() noexcept
		{
			return axes;
		}

		[[nodiscard]] float& axis(const uint8_t i) noexcept
		{
			if (i == 1)
			{
				return ((T*)this)->y;
			}
			if constexpr (axes > 2)
			{
				if (i == 2)
				{
					return ((T*)this)->z;
				}
				if constexpr (axes > 3)
				{
					if (i == 3)
					{
						return ((T*)this)->w;
					}
				}
			}
			return ((T*)this)->x;
		}

		[[nodiscard]] float& operator[] (const uint8_t i) noexcept
		{
			return axis(i);
		}

		[[nodiscard]] float axis(const uint8_t i) const noexcept
		{
			if (i == 1)
			{
				return ((const T*)this)->y;
			}
			if constexpr (axes > 2)
			{
				if (i == 2)
				{
					return ((const T*)this)->z;
				}
				if constexpr (axes > 3)
				{
					if (i == 3)
					{
						return ((const T*)this)->w;
					}
				}
			}
			return ((const T*)this)->x;
		}

		[[nodiscard]] float operator[] (const uint8_t i) const noexcept
		{
			return axis(i);
		}

		template <typename T2>
		[[nodiscard]] T operator+(const T2& b) const noexcept
		{
			T res(*((const T*)this));
			res += b;
			return res;
		}

		template <typename T2>
		void operator+=(const T2& b) noexcept
		{
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				axis(i) += b[i];
			}
		}

		[[nodiscard]] T operator+(const float f) const noexcept
		{
			T res(*((const T*)this));
			res += f;
			return res;
		}

		void operator+=(const float f) noexcept
		{
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				axis(i) += f;
			}
		}

		template <typename T2>
		[[nodiscard]] T operator-(const T2& b) const noexcept
		{
			T res(*((const T*)this));
			res -= b;
			return res;
		}

		template <typename T2>
		void operator-=(const T2& b) noexcept
		{
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				axis(i) -= b[i];
			}
		}

		[[nodiscard]] T operator-(const float f) const noexcept
		{
			T res(*((const T*)this));
			res -= f;
			return res;
		}

		void operator-=(const float f) noexcept
		{
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				axis(i) -= f;
			}
		}

		template <typename T2>
		[[nodiscard]] T operator*(const T2& b) const noexcept
		{
			T res(*((const T*)this));
			res *= b;
			return res;
		}

		template <typename T2>
		void operator*=(const T2& b) noexcept
		{
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				axis(i) *= b[i];
			}
		}

		[[nodiscard]] T operator*(const float f) const noexcept
		{
			T res(*((const T*)this));
			res *= f;
			return res;
		}

		void operator*=(const float f) noexcept
		{
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				axis(i) *= f;
			}
		}

		[[nodiscard]] T operator/(const float f) const noexcept
		{
			T res(*((const T*)this));
			res /= f;
			return res;
		}

		void operator/=(const float f) noexcept
		{
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				axis(i) /= f;
			}
		}

		template <typename T2>
		[[nodiscard]] bool operator==(const T2& b) const noexcept
		{
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				if (axis(i) != b[i])
				{
					return false;
				}
			}
			return true;
		}

		template <typename T2>
		[[nodiscard]] bool operator!=(const T2& b) const noexcept
		{
			return !operator==(b);
		}

		[[nodiscard]] bool isNull() const noexcept
		{
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				if (axis(i) != 0.0f)
				{
					return false;
				}
			}
			return true;
		}

		void reset() noexcept
		{
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				axis(i) = 0.0f;
			}
		}

		[[nodiscard]] float magnitude() const noexcept
		{
			float accum = 0.0f;
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				accum += pow2(axis(i));
			}
			return sqrtf(accum);
		}

		template <typename T2>
		[[nodiscard]] float distance(const T2& b) const noexcept
		{
			return (*this - b).magnitude();
		}

		[[nodiscard]] T abs() const noexcept
		{
			T res;
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				res[i] = fabsf(axis(i));
			}
			return res;
		}

		[[nodiscard]] float sum() const noexcept
		{
			float accum = 0.0f;
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				accum += axis(i);
			}
			return accum;
		}

		[[nodiscard]] float min() const noexcept
		{
			float min = FLT_MAX;
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				if (axis(i) < min)
				{
					min = axis(i);
				}
			}
			return min;
		}

		[[nodiscard]] float max() const noexcept
		{
			float max = 0.0f;
			for (uint8_t i = 0; i != getAxes(); ++i)
			{
				if (axis(i) > max)
				{
					max = axis(i);
				}
			}
			return max;
		}

		template <typename T2>
		[[nodiscard]] float dot(const T2& b) const noexcept
		{
			return (*this * b).sum();
		}

		void normalise() noexcept
		{
			operator *= (1.0f / magnitude());
		}
	};
}
