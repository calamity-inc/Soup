#pragma once

#include <cstdint>

#include "Reader.hpp"

namespace soup
{
	class BitReader
	{
	public:
		Reader* r;
		uint8_t bit_idx = 0;
		uint8_t byte;

		BitReader(Reader* r)
			: r(r)
		{
		}

		[[nodiscard]] Reader* getStream() const noexcept
		{
			return r;
		}

		[[nodiscard]] constexpr bool isByteAligned() const noexcept
		{
			return bit_idx == 0;
		}

		bool finishByte() noexcept;

	protected:
		void ignore(uint8_t bits);
		bool forward(uint8_t bits);

	public:
		template <uint8_t Bits, typename T>
		bool t(T& out)
		{
			if constexpr (Bits == 1)
			{
				bool b;
				if (!b(b))
				{
					return false;
				}
				out = b;
				return true;
			}
			return u8(Bits, out);
		}

		bool b(bool& out);
		bool u8(uint8_t bits, uint8_t& out);
	};
}
