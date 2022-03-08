#pragma once

#include "packet_io_base.hpp"

#include <istream>
#include <vector>

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
		bool str_lp_u8(std::string& v, const uint8_t max_len = 0xFF)
		{
			uint8_t len;
			return u8(len) && len <= max_len && str_lp_impl(v, len);
		}

		// Length-prefixed string, using u16 for the length prefix.
		bool str_lp_u16(std::string& v, const uint16_t max_len = 0xFFFF)
		{
			uint16_t len;
			return u16(len) && len <= max_len && str_lp_impl(v, len);
		}

		// Length-prefixed string, using u24 for the length prefix.
		bool str_lp_u24(std::string& v, const uint32_t max_len = 0xFFFFFF)
		{
			uint32_t len;
			return u24(len) && len <= max_len && str_lp_impl(v, len);
		}

		// Length-prefixed string, using u32 for the length prefix.
		bool str_lp_u32(std::string& v, const uint32_t max_len = 0xFFFFFFFF)
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

		// std::vector<uint8_t> with u8 size prefix.
		bool vec_u8_u8(std::vector<uint8_t>& v)
		{
			uint8_t len;
			if (!u8(len))
			{
				return false;
			}
			v.clear();
			v.reserve(len);
			for (; len; --len)
			{
				uint8_t entry;
				if (!u8(entry))
				{
					return false;
				}
				v.emplace_back(std::move(entry));
			}
			return true;
		}

		// std::vector<uint16_t> with u16 size prefix.
		bool vec_u16_u16(std::vector<uint16_t>& v)
		{
			uint16_t len;
			if (!u16(len))
			{
				return false;
			}
			v.clear();
			v.reserve(len);
			for (; len; --len)
			{
				uint16_t entry;
				if (!u16(entry))
				{
					return false;
				}
				v.emplace_back(std::move(entry));
			}
			return true;
		}

		// std::vector<uint16_t> with u16 byte length prefix.
		bool vec_u16_bl_u16(std::vector<uint16_t>& v)
		{
			uint16_t len;
			if (!u16(len))
			{
				return false;
			}
			v.clear();
			v.reserve(len);
			for (; len >= sizeof(uint16_t); len -= sizeof(uint16_t))
			{
				uint16_t entry;
				if (!u16(entry))
				{
					return false;
				}
				v.emplace_back(std::move(entry));
			}
			return true;
		}

		// vector of str_lp_u24 with u24 byte length prefix.
		bool vec_str_lp_u24_bl_u24(std::vector<std::string>& v)
		{
			uint32_t len;
			if (!u24(len))
			{
				return false;
			}
			v.clear();
			v.reserve(len);
			while (len >= 3)
			{
				std::string entry;
				if (!str_lp_u24(entry))
				{
					return false;
				}
				len -= ((uint32_t)entry.size() + 3);
				v.emplace_back(std::move(entry));
			}
			return true;
		}
	};
}
