#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "endianness.hpp"

namespace soup
{
	class io_base
	{
	protected:
		const bool native_endianness;

		io_base(bool little_endian)
			: native_endianness(SOUP_LITTLE_ENDIAN == little_endian)
		{
		}

	public:
		virtual ~io_base() = default;

		[[nodiscard]] virtual bool isRead() const = 0;

		[[nodiscard]] bool isWrite() const
		{
			return !isRead();
		}

		[[nodiscard]] virtual bool hasMore()
		{
			return true;
		}

		virtual bool u8(uint8_t& v) = 0;

		bool b(bool& v)
		{
			return u8(*(uint8_t*)&v);
		}

		bool c(char& v)
		{
			return u8(*(uint8_t*)&v);
		}

		bool u16(uint16_t& v)
		{
			if (native_endianness)
			{
				return u8(((uint8_t*)&v)[0])
					&& u8(((uint8_t*)&v)[1]);
			}
			else
			{
				return u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[0]);
			}
		}

		bool u24(uint32_t& v)
		{
			if (isRead())
			{
				v = 0;
			}
			if (native_endianness)
			{
				return u8(((uint8_t*)&v)[0])
					&& u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[2]);
			}
			else
			{
				return u8(((uint8_t*)&v)[2])
					&& u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[0]);
			}
		}

		bool u32(uint32_t& v)
		{
			if (native_endianness)
			{
				return u8(((uint8_t*)&v)[0])
					&& u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[2])
					&& u8(((uint8_t*)&v)[3]);
			}
			else
			{
				return u8(((uint8_t*)&v)[3])
					&& u8(((uint8_t*)&v)[2])
					&& u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[0]);
			}
		}

		bool u40(uint64_t& v)
		{
			if (isRead())
			{
				v = 0;
			}
			if (native_endianness)
			{
				return u8(((uint8_t*)&v)[0])
					&& u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[2])
					&& u8(((uint8_t*)&v)[3])
					&& u8(((uint8_t*)&v)[4]);
			}
			else
			{
				return u8(((uint8_t*)&v)[4])
					&& u8(((uint8_t*)&v)[3])
					&& u8(((uint8_t*)&v)[2])
					&& u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[0]);
			}
		}

		bool u48(uint64_t& v)
		{
			if (isRead())
			{
				v = 0;
			}
			if (native_endianness)
			{
				return u8(((uint8_t*)&v)[0])
					&& u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[2])
					&& u8(((uint8_t*)&v)[3])
					&& u8(((uint8_t*)&v)[4])
					&& u8(((uint8_t*)&v)[5]);
			}
			else
			{
				return u8(((uint8_t*)&v)[5])
					&& u8(((uint8_t*)&v)[4])
					&& u8(((uint8_t*)&v)[3])
					&& u8(((uint8_t*)&v)[2])
					&& u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[0]);
			}
		}

		bool u56(uint64_t& v)
		{
			if (isRead())
			{
				v = 0;
			}
			if (native_endianness)
			{
				return u8(((uint8_t*)&v)[0])
					&& u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[2])
					&& u8(((uint8_t*)&v)[3])
					&& u8(((uint8_t*)&v)[4])
					&& u8(((uint8_t*)&v)[5])
					&& u8(((uint8_t*)&v)[6]);
			}
			else
			{
				return u8(((uint8_t*)&v)[6])
					&& u8(((uint8_t*)&v)[5])
					&& u8(((uint8_t*)&v)[4])
					&& u8(((uint8_t*)&v)[3])
					&& u8(((uint8_t*)&v)[2])
					&& u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[0]);
			}
		}

		bool u64(uint64_t& v)
		{
			if (native_endianness)
			{
				return u8(((uint8_t*)&v)[0])
					&& u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[2])
					&& u8(((uint8_t*)&v)[3])
					&& u8(((uint8_t*)&v)[4])
					&& u8(((uint8_t*)&v)[5])
					&& u8(((uint8_t*)&v)[6])
					&& u8(((uint8_t*)&v)[7]);
			}
			else
			{
				return u8(((uint8_t*)&v)[7])
					&& u8(((uint8_t*)&v)[6])
					&& u8(((uint8_t*)&v)[5])
					&& u8(((uint8_t*)&v)[4])
					&& u8(((uint8_t*)&v)[3])
					&& u8(((uint8_t*)&v)[2])
					&& u8(((uint8_t*)&v)[1])
					&& u8(((uint8_t*)&v)[0]);
			}
		}

		[[nodiscard]] bool i8(int8_t& v)
		{
			return u8(*(uint8_t*)&v);
		}

		[[nodiscard]] bool i16(int16_t& v)
		{
			return u16(*(uint16_t*)&v);
		}

		[[nodiscard]] bool i32(int32_t& v)
		{
			return u32(*(uint32_t*)&v);
		}

		[[nodiscard]] bool i64(int64_t& v)
		{
			return u64(*(uint64_t*)&v);
		}
	};
}
