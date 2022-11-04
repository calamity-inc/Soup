#include "Writer.hpp"

#include "Bigint.hpp"

namespace soup
{
	bool Writer::om_bigint(const Bigint& v)
	{
		auto chunks = 0;
		{
			Bigint val = v;
			while (true)
			{
				val >>= 7;
				if (!val)
				{
					break;
				}
				++chunks;
			}
		}
		do
		{
			auto tmp = (v >> (size_t)(chunks * 7));
			uint8_t byte = (tmp.getChunk(0) & 0x7F);
			if (chunks != 0)
			{
				byte |= 0x80;
			}
			u8(byte);
		} while (chunks--);
		return true;
	}

	bool Writer::bigint_lp_u64_dyn(Bigint& v)
	{
		return str_lp_u64_dyn(v.toBinary());
	}
}
