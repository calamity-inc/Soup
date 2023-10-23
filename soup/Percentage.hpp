#pragma once

#include <type_traits>

namespace soup
{
	template <typename T>
	struct Percentage
	{
		static_assert(std::is_unsigned_v<T>);

		T value;

		constexpr Percentage(T value) noexcept
			: value(value)
		{
		}

		constexpr Percentage(float fvalue) noexcept
			: value(static_cast<T>(fvalue * static_cast<float>((T)-1)))
		{
		}

		void operator =(T value) noexcept
		{
			this->value = value;
		}

		operator T& () noexcept
		{
			return value;
		}

		operator const T& () const noexcept
		{
			return value;
		}

		[[nodiscard]] float toFloat() const noexcept
		{
			return static_cast<float>(value) / static_cast<float>((T)-1);
		}

		Percentage<T> operator*(T b) const noexcept
		{
			return operator*=(Percentage<T>(b));
		}

		Percentage<T> operator*(const Percentage<T>& b) const noexcept
		{
			return static_cast<T>((static_cast<unsigned int>(value) * static_cast<unsigned int>(b.value)) / (T)-1);
		}

		void operator*=(T b) noexcept
		{
			return operator*=(Percentage<T>(b));
		}

		void operator*=(const Percentage<T>& b) noexcept
		{
			value = static_cast<T>((static_cast<unsigned int>(value) * static_cast<unsigned int>(b.value)) / (T)-1);
		}
	};
}
