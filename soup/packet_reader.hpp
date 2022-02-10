#pragma once

#include "packet_io_base.hpp"

#include <istream>

namespace soup
{
	class packet_reader : public packet_io_base<packet_reader>
	{
	public:
		std::basic_istream<char, std::char_traits<char>>* is;

		packet_reader(std::basic_istream<char, std::char_traits<char>>* is)
			: is(is)
		{
		}

		[[nodiscard]] static constexpr bool isRead() noexcept
		{
			return true;
		}

		[[nodiscard]] static constexpr bool isWrite() noexcept
		{
			return false;
		}

		[[nodiscard]] bool hasMore()
		{
			return is->peek() != EOF;
		}

		[[nodiscard]] bool u8(uint8_t& p)
		{
			return !is->read((char*)&p, sizeof(uint8_t)).fail();
		}

		// An unsigned 64-bit integer encoded in 1..9 bytes. The most significant bit of bytes 1 to 8 is used to indicate if another byte follows.
		[[nodiscard]] bool u64_dyn(uint64_t& v)
		{
			v = 0;
			uint8_t bits = 0;
			while (true)
			{
				uint8_t b;
				if (!u8(b))
				{
					return false;
				}
				bool has_next = false;
				if ((bits < (64 - 8)) && (b & 0x80))
				{
					has_next = true;
					b &= 0x7F;
				}
				v |= ((uint64_t)b << bits);
				if (!has_next)
				{
					break;
				}
				bits += 7;
			}
			return true;
		}
		
		// Null-terminated string.
		[[nodiscard]] bool str_nt(std::string& v)
		{
			v.clear();
			while (true)
			{
				char c;
				if (!packet_io_base::c(c))
				{
					return false;
				}
				if (c == 0)
				{
					break;
				}
				v.push_back(c);
			}
			return true;
		}

	protected:
		bool str_lp_impl(std::string& v, size_t len)
		{
			auto data = new char[len];
			is->read(data, len);
			v.assign(data, len);
			delete[] data;
			return !is->fail();
		}

	public:
		// Length-prefixed string, using u64_dyn for the length prefix.
		bool str_lp_u64_dyn(std::string& v)
		{
			uint64_t len;
			return u64_dyn(len) && str_lp_impl(v, len);
		}

		// Length-prefixed string, using u8 for the length prefix.
		bool str_lp_u8(std::string& v, const size_t max_len = 0xFF)
		{
			uint8_t len;
			return u8(len) && len <= max_len && str_lp_impl(v, len);
		}

		// Length-prefixed string, using u16 for the length prefix.
		bool str_lp_u16(std::string& v, const size_t max_len = 0xFFFF)
		{
			uint16_t len;
			return u16(len) && len <= max_len && str_lp_impl(v, len);
		}

		// Length-prefixed string, using u32 for the length prefix.
		bool str_lp_u32(std::string& v, const size_t max_len = 0xFFFFFFFF)
		{
			uint32_t len;
			return u32(len) && len <= max_len && str_lp_impl(v, len);
		}

		// Length-prefixed string, using u64 for the length prefix.
		bool str_lp_u64(std::string& v)
		{
			uint64_t len;
			return u64(len) && str_lp_impl(v, len);
		}
	};
}
