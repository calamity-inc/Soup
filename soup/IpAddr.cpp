#include "IpAddr.hpp"

#include "Endian.hpp"
#include "intutil.hpp"

namespace soup
{
	bool IpAddr::fromString(const std::string& str)
	{
		if (str.find('.') == std::string::npos)
		{
			return inet_pton(AF_INET6, str.data(), &data) == 1;
		}
		else
		{
			setV4();
			return inet_pton(AF_INET, str.data(), reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&data) + 12)) == 1;
		}
	}

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
