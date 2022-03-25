#pragma once

#include <cstdint>

#include "endianness.hpp"

namespace soup
{
	template <typename T, bool little_endian>
	struct packet_io_base
	{
		static constexpr bool native_endianness = (SOUP_LITTLE_ENDIAN == little_endian);

		[[nodiscard]] bool b(bool& v)
		{
			return reinterpret_cast<T*>(this)->u8(*(uint8_t*)&v);
		}

		[[nodiscard]] bool c(char& v)
		{
			return reinterpret_cast<T*>(this)->u8(*(uint8_t*)&v);
		}

		[[nodiscard]] bool u16(uint16_t& v)
		{
			if constexpr (native_endianness)
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1]);
			}
			else
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0]);
			}
		}

		[[nodiscard]] bool u24(uint32_t& v)
		{
			if (T::isRead())
			{
				v = 0;
			}
			if constexpr (native_endianness)
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[2]);
			}
			else
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[2])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0]);
			}
		}
		
		[[nodiscard]] bool u32(uint32_t& v)
		{
			if constexpr (native_endianness)
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[2])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[3]);
			}
			else
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[3])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[2])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0]);
			}
		}

		[[nodiscard]] bool u40(uint64_t& v)
		{
			if (T::isRead())
			{
				v = 0;
			}
			if constexpr (native_endianness)
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[2])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[3])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[4]);
			}
			else
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[4])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[3])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[2])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0]);
			}
		}
		
		[[nodiscard]] bool u48(uint64_t& v)
		{
			if (T::isRead())
			{
				v = 0;
			}
			if constexpr (native_endianness)
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[2])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[3])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[4])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[5]);
			}
			else
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[5])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[4])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[3])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[2])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0]);
			}
		}

		[[nodiscard]] bool u56(uint64_t& v)
		{
			if (T::isRead())
			{
				v = 0;
			}
			if constexpr (native_endianness)
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[2])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[3])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[4])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[5])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[6]);
			}
			else
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[6])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[5])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[4])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[3])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[2])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0]);
			}
		}

		[[nodiscard]] bool u64(uint64_t& v)
		{
			if constexpr (native_endianness)
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[2])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[3])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[4])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[5])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[6])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[7]);
			}
			else
			{
				return reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[7])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[6])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[5])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[4])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[3])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[2])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[1])
					&& reinterpret_cast<T*>(this)->u8(((uint8_t*)&v)[0]);
			}
		}

		[[nodiscard]] bool i8(int8_t& v)
		{
			return reinterpret_cast<T*>(this)->u8(*(uint8_t*)&v);
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
