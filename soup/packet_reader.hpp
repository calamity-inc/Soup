#pragma once

#include "packet_io_base.hpp"

#include <istream>

namespace soup
{
	struct packet_reader : public packet_io_base<packet_reader>
	{
		std::basic_istream<char, std::char_traits<char>>* is;

		packet_reader(std::basic_istream<char, std::char_traits<char>>* is)
			: is(is)
		{
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
	};
}
