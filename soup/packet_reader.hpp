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

		bool u8(uint8_t& p)
		{
			return !is->read((char*)&p, sizeof(uint8_t)).fail();
		}
	};
}
