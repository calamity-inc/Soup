#pragma once

#include "CidrSubnetInterface.hpp"

#include "CidrSubnet6.hpp"

namespace soup
{
	struct CidrSubnet6Interface : public CidrSubnetInterface
	{
		CidrSubnet6 subnet;

		CidrSubnet6Interface(const IpAddr& addr, uint8_t size)
			: subnet(addr, size)
		{
		}

		[[nodiscard]] bool contains(const IpAddr& addr) const noexcept final
		{
			return addr.isV4() == false
				&& subnet.contains(addr)
				;
		}

		[[nodiscard]] bool isV4() const noexcept final
		{
			return false;
		}

		[[nodiscard]] IpAddr getAddr() const noexcept final
		{
			return subnet.getAddr();
		}

		[[nodiscard]] uint8_t getSize() const noexcept final
		{
			return subnet.getSize();
		}
	};
}
