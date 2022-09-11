#include "CidrSubnet6.hpp"

#include "rand.hpp"

namespace soup
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
}
