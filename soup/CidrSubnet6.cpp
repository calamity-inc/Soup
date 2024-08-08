#include "CidrSubnet6.hpp"

#include "bitutil.hpp"
#include "rand.hpp"

NAMESPACE_SOUP
{
	std::array<uint32_t, 4> CidrSubnet6::random() const noexcept
	{
		auto addr = this->addr;
		for (auto i = 0; i != 4; ++i)
		{
			addr[i] |= rand.t<uint32_t>(0, ~mask[i]);
		}
		return addr;
	}

	uint8_t CidrSubnet6::getSize() const noexcept
	{
		uint8_t size = 0;
		for (const auto& part : mask)
		{
			if (part == 0)
			{
				break;
			}
			size += static_cast<uint8_t>(32 - bitutil::getLeastSignificantSetBit(part));
		}
		return size;
	}
}
