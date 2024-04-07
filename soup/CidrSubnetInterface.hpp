#pragma once

#include <string>

#include "fwd.hpp"
#include "UniquePtr.hpp"

NAMESPACE_SOUP
{
	struct CidrSubnetInterface
	{
		[[nodiscard]] static UniquePtr<CidrSubnetInterface> construct(const std::string& str);
		[[nodiscard]] static UniquePtr<CidrSubnetInterface> construct(const IpAddr& addr, uint8_t size);

		virtual ~CidrSubnetInterface() = default;

		[[nodiscard]] virtual bool contains(const IpAddr& addr) const noexcept = 0;
		[[nodiscard]] bool contains(const CidrSubnetInterface& b) const noexcept;

		[[nodiscard]] virtual bool isV4() const noexcept = 0;
		[[nodiscard]] virtual IpAddr getAddr() const noexcept = 0;
		[[nodiscard]] virtual uint8_t getSize() const noexcept = 0;
		[[nodiscard]] std::string toString() const;
	};
}
