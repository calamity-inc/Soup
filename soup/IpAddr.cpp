#include "IpAddr.hpp"

#include "endianness.hpp"
#include "intutil.hpp"

namespace soup
{
	uint32_t IpAddr::getV4NativeEndian() const noexcept
	{
		auto v4 = getV4();
		if constexpr (SOUP_LITTLE_ENDIAN)
		{
			v4 = intutil::invertEndianness(getV4());
		}
		return v4;
	}
}
