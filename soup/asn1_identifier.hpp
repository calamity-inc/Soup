#pragma once

#include <cstdint>
#include <string>

#include "stream.hpp"

namespace soup
{
	struct asn1_identifier
	{
		uint8_t m_class;
		bool constructed;
		uint32_t type;

		[[nodiscard]] std::string toDer() const
		{
			std::string ret{};
			uint8_t first = (((m_class << 1) | constructed) << 5);
			if (type <= 30)
			{
				first |= type;
				ret.push_back(first);
			}
			else
			{
				first |= 31;
				ret.push_back(first);
				stream::writeOmInt(ret, type);
			}
			return ret;
		}
	};
}
