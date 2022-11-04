#include "Reader.hpp"

#include "Bigint.hpp"

namespace soup
{
	bool Reader::om_bigint(Bigint& v)
	{
		return om<Bigint>(v);
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
