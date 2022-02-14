#pragma once

namespace soup
{
	template <size_t Size>
	struct string_literal
	{
		char data[Size];

		consteval string_literal(const char(&in)[Size])
		{
			for (size_t i = 0; i != Size; ++i)
			{
				data[i] = in[i];
			}
		}

		[[nodiscard]] consteval size_t size() const noexcept
		{
			return Size;
		}

		[[nodiscard]] consteval const char* c_str() const noexcept
		{
			return &data[0];
		}
	};
}
