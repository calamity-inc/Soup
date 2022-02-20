#pragma once

#include <cstdint>

namespace soup
{
	struct asn1_identifier
	{
		uint8_t m_class;
		bool constructed;
		uint32_t type;
	};
}
