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

		bool u8(uint8_t& v)
		{
			os->write((const char*)&v, sizeof(uint8_t));
			return true;
		}
	};
}
