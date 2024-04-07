#pragma once

#include "CidrSubnetInterface.hpp"

#include "CidrSubnet4.hpp"

NAMESPACE_SOUP
{
	struct CidrSubnet4Interface : public CidrSubnetInterface
	{
		CidrSubnet4 subnet;

		CidrSubnet4Interface(const IpAddr& addr, uint8_t size)
			: subnet(addr, size)
		{
		}

		[[nodiscard]] bool contains(const IpAddr& addr) const noexcept final
		{
			return addr.isV4() == true
				&& subnet.contains(addr)
				;
			;
		}

		[[nodiscard]] bool isV4() const noexcept final
		{
			return true;
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
