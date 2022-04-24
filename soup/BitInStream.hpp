#pragma once

#include <cstdint>
#include <istream>

namespace soup
{
	class BitInStream
	{
	protected:
		std::istream* is;
		uint8_t bit_idx;
		uint8_t byte;

	public:
		BitInStream(std::istream* is)
			: is(is)
		{
		}

		[[nodiscard]] constexpr bool isByteAligned() const noexcept
		{
			return bit_idx == 0;
		}

		void nextByte() noexcept
		{
			bit_idx = 0;
		}

		void ignore(uint8_t bits)
		{
			if (isByteAligned())
			{
				is->read((char*)&byte, 1);
			}
			forward(bits);
		}

	protected:
		bool forward(uint8_t bits)
		{
			bits %= 8;
			bit_idx += bits;
			if (bit_idx == 8)
			{
				bit_idx = 0;
			}
			else if (bit_idx > 8)
			{
				bit_idx -= 8;
				is->read((char*)&byte, 1);
				if (is->bad() || is->eof())
				{
					bit_idx = 0;
					return false;
				}
			}
			return true;
		}

	public:
		template <uint8_t Bits, typename T>
		bool read(T& out)
		{
			if constexpr (Bits == 1)
			{
				bool b;
				if (!readBit(b))
				{
					return false;
				}
				out = b;
				return true;
			}
			else
			{
				return readByte(Bits, out);
			}
			return false;
		}

		bool readBit(bool& out)
		{
			if (isByteAligned())
			{
				bit_idx = 1;
				is->read((char*)&byte, 1);
				if (is->bad() || is->eof())
				{
					return false;
				}
				out = byte & 1;
			}
			else
			{
				out = (byte >> bit_idx) & 1;
				if (++bit_idx == 8)
				{
					bit_idx = 0;
				}
			}
			return true;
		}

		[[nodiscard]] uint8_t readByte(uint8_t bits, uint8_t& out)
		{
			if (isByteAligned())
			{
				bit_idx = bits;
				is->read((char*)&byte, 1);
				if (is->bad() || is->eof())
				{
					bit_idx = 0;
					return false;
				}
				out = (byte & ((1 << bits) - 1));
			}
			else
			{
				out = ((byte >> bit_idx) & ((1 << bits) - 1));
				if (bit_idx + bits > 8)
				{
					auto readable_bits = (8 - bit_idx);
					auto read_bits = (bits - readable_bits);
					auto next_byte_bits = (bit_idx - (8 - bits));
					out <<= next_byte_bits;
					if (forward(read_bits))
					{
						uint8_t next_byte;
						if (readByte(next_byte_bits, next_byte))
						{
							out |= next_byte;
						}
					}
				}
				else
				{
					forward(bits);
				}
			}
			return true;
		}
	};
}
