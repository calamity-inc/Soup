#include "IpAddr.hpp"

#include "dnsOsResolver.hpp"
#include "Endian.hpp"
#include "string.hpp"

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
			maskToV4();
			return inet_pton(AF_INET, str.data(), reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&data) + 12)) == 1;
		}
	}

	uint32_t IpAddr::getV4NativeEndian() const noexcept
	{
		auto v4 = getV4();
		if constexpr (NATIVE_ENDIAN == LITTLE_ENDIAN)
		{
			v4 = Endianness::invert(getV4());
		}
		return v4;
	}

	std::string IpAddr::getArpaName() const
	{
		if (isV4())
		{
			auto v4 = getV4();
			std::string str = std::to_string((v4 >> 24) & 0xFF);
			str.push_back('.');
			str.append(std::to_string((v4 >> 16) & 0xFF));
			str.push_back('.');
			str.append(std::to_string((v4 >> 8) & 0xFF));
			str.push_back('.');
			str.append(std::to_string(v4 & 0xFF));
			str.append(".in-addr.arpa");
			return str;
		}

		std::string str;
		for (const auto& b : string::bin2hexLower(std::string(reinterpret_cast<const char*>(&data), 16)))
		{
			str.insert(0, 1, '.');
			str.insert(0, 1, b);
		}
		str.append("in6.arpa");
		return str;
	}

	std::string IpAddr::getReverseDns() const
	{
		dnsOsResolver resolver;
		return getReverseDns(resolver);
	}

	std::string IpAddr::getReverseDns(dnsResolver& resolver) const
	{
		for (const auto& record : resolver.lookup(DNS_PTR, getArpaName()))
		{
			if (record->type == DNS_PTR)
			{
				return reinterpret_cast<dnsPtrRecord*>(record.get())->data;
			}
		}
		return {};
	}
}
