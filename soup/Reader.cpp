#include "Reader.hpp"

#include "Bigint.hpp"

namespace soup
{
	bool Reader::om_bigint(Bigint& v)
	{
		Bigint val{};
		while (hasMore())
		{
			uint8_t byte;
			u8(byte);
			val <<= 7;
			val |= (Bigint::chunk_t)(byte & 0x7F);
			if (!(byte & 0x80))
			{
				break;
			}
		}
		v = val;
		return true;
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
