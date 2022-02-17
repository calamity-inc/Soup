#pragma once

#include "packet_io_base.hpp"

#include <ostream>
#include <vector>

namespace soup
{
	struct packet_writer : public packet_io_base<packet_writer>
	{
		std::basic_ostream<char, std::char_traits<char>>* os;

		packet_writer(std::basic_ostream<char, std::char_traits<char>>* os)
			: os(os)
		{
		}

		[[nodiscard]] static constexpr bool isRead() noexcept
		{
			return false;
		}

		[[nodiscard]] static constexpr bool isWrite() noexcept
		{
			return true;
		}

		[[nodiscard]] static constexpr bool hasMore() noexcept
		{
			return true;
		}

		bool u8(const uint8_t& v)
		{
			os->write((const char*)&v, sizeof(uint8_t));
			return true;
		}

		// An unsigned 64-bit integer encoded in 1..9 bytes. The most significant bit of bytes 1 to 8 is used to indicate if another byte follows.
		bool u64_dyn(const uint64_t& v)
		{
			uint64_t in = v;
			for (uint8_t i = 0; i < 8; i++)
			{
				uint8_t cur = (in & 0x7F);
				in >>= 7;
				if (in != 0)
				{
					cur |= 0x80;
				}
				u8(cur);
				if (in == 0)
				{
					return true;
				}
			}
			if (in != 0)
			{
				u8((uint8_t)in);
			}
			return true;
		}

		// Null-terminated string.
		bool str_nt(std::string& v)
		{
			os->write(v.data(), v.size());
			u8(0);
			return true;
		}

		// Length-prefixed string, using u64_dyn for the length prefix.
		bool str_lp_u64_dyn(std::string& v)
		{
			u64_dyn(v.size());
			os->write(v.data(), v.size());
			return true;
		}

		// Length-prefixed string, using u8 for the length prefix.
		bool str_lp_u8(std::string& v, const uint8_t max_len = 0xFF)
		{
			size_t len = v.size();
			if (len <= max_len)
			{
				auto tl = (uint8_t)len;
				u8(tl);
				os->write(v.data(), v.size());
				return true;
			}
			return false;
		}

		// Length-prefixed string, using u16 for the length prefix.
		bool str_lp_u16(std::string& v, const uint16_t max_len = 0xFFFF)
		{
			size_t len = v.size();
			if (len <= max_len)
			{
				auto tl = (uint16_t)len;
				if (u16(tl))
				{
					os->write(v.data(), v.size());
					return true;
				}
			}
			return false;
		}

		// Length-prefixed string, using u32 for the length prefix.
		bool str_lp_u32(std::string& v, const uint32_t max_len = 0xFFFFFFFF)
		{
			size_t len = v.size();
			if (len <= max_len)
			{
				auto tl = (uint32_t)len;
				if (u32(tl))
				{
					os->write(v.data(), v.size());
					return true;
				}
			}
			return false;
		}

		// Length-prefixed string, using u64 for the length prefix.
		bool str_lp_u64(std::string& v)
		{
			size_t len = v.size();
			if (u64(len))
			{
				os->write(v.data(), v.size());
				return true;
			}
			return false;
		}

		// std::vector<uint8_t> with u8 size prefix.
		bool vec_u8_u8(std::vector<uint8_t>& v)
		{
			if (v.size() > 0xFF)
			{
				return false;
			}
			auto len = (uint8_t)v.size();
			if (!u8(len))
			{
				return false;
			}
			for (auto& entry : v)
			{
				if (!u8(entry))
				{
					return false;
				}
			}
			return true;
		}

		// std::vector<uint16_t> with u16 size prefix.
		bool vec_u16_u16(std::vector<uint16_t>& v)
		{
			if (v.size() > 0xFFFF)
			{
				return false;
			}
			auto len = (uint16_t)v.size();
			if (!u16(len))
			{
				return false;
			}
			for (auto& entry : v)
			{
				if (!u16(entry))
				{
					return false;
				}
			}
			return true;
		}

		// std::vector<uint16_t> with u16 byte length prefix.
		bool vec_u16_bl_u16(std::vector<uint16_t>& v)
		{
			size_t bl = (v.size() * sizeof(uint16_t));
			if (bl > 0xFFFF)
			{
				return false;
			}
			auto bl_u16 = (uint16_t)bl;
			if (!u16(bl_u16))
			{
				return false;
			}
			for (auto& entry : v)
			{
				if (!u16(entry))
				{
					return false;
				}
			}
			return true;
		}
	};
}
