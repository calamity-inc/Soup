#pragma once

#include "drString.hpp"

#include "drInt.hpp"
#include "IpAddr.hpp"

namespace soup
{
	struct drIpAddr : public drString
	{
		IpAddr ipaddr_data;

		drIpAddr(std::string&& string_data, IpAddr&& ipaddr_data)
			: drString(std::move(string_data)), ipaddr_data(std::move(ipaddr_data))
		{
		}

		[[nodiscard]] const char* getTypeName() const noexcept final
		{
			return "IP address";
		}

		[[nodiscard]] std::vector<drAdaptor> getAdaptors() const final
		{
			auto adaptors = drString::getAdaptors();
			if (ipaddr_data.isV4())
			{
				adaptors.emplace_back(drAdaptor{ "int", &adaptor_toInt });
			}
			return adaptors;
		}

	private:
		static UniquePtr<drData> adaptor_toInt(const drData& data, const Capture&)
		{
			return soup::make_unique<drInt>(static_cast<const drIpAddr&>(data).ipaddr_data.getV4NativeEndian());
		}
	};
}
