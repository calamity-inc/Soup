#pragma once

#include "scoped_enum.hpp"

namespace soup
{
	SCOPED_ENUM(asn1_type, uint32_t,
		_BOOLEAN = 1,
		INTEGER = 2,
		BITSTRING = 3,
		STRING_OCTET = 4,
		_NULL = 5,
		OID = 6,
		SEQUENCE = 16,
		SET = 17,
		STRING_PRINTABLE = 19,
		STRING_IA5 = 22,
		UTCTIME = 23,
	);
}
