#include "Reader.hpp"

#include "Bigint.hpp"

namespace soup
{
	bool Reader::om_bigint(Bigint& v)
	{
		v.reset();
		uint8_t byte;
		while (u8(byte))
		{
			v <<= 7;
			v |= (Bigint::chunk_t)(byte & 0x7F);
			if (!(byte & 0x80))
			{
				return true;
			}
		}
		return false;
	}

	bool Reader::bigint_lp_u64_dyn(Bigint& v)
	{
		std::string str;
		if (str_lp_u64_dyn(str))
		{
			v = Bigint::fromBinary(str);
			return true;
		}
		return false;
	}
}
