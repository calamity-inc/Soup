#pragma once

#include "packet_io_base.hpp"

#include <ostream>

namespace soup
{
	struct packet_writer : public packet_io_base<packet_writer>
	{
		std::basic_ostream<char, std::char_traits<char>>* os;

		packet_writer(std::basic_ostream<char, std::char_traits<char>>* os)
			: os(os)
		{
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
	};
}
