#include "IpAddr.hpp"

#include "Endian.hpp"
#include "intutil.hpp"

namespace soup
{
	uint32_t IpAddr::getV4NativeEndian() const noexcept
	{
		auto v4 = getV4();
		if constexpr (NATIVE_ENDIAN == LITTLE_ENDIAN)
		{
			v4 = intutil::invertEndianness(getV4());
		}
		return v4;
	}
}
