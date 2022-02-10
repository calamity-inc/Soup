#pragma once

namespace soup
{
	template <size_t Len>
	struct string_literal
	{
		char data[Len];

		consteval string_literal(const char(&in)[Len])
		{
			for (size_t i = 0; i != Len; ++i)
			{
				data[i] = in[i];
			}
		}

		[[nodiscard]] consteval size_t size() const noexcept
		{
			return Len;
		}

		[[nodiscard]] consteval const char* c_str() const noexcept
		{
			return &data[0];
		}
	};
}
