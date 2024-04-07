#pragma once

#include "drData.hpp"

#include "string.hpp"

NAMESPACE_SOUP
{
	struct drInt : public drData
	{
		int64_t int_data;

		drInt(int64_t data)
			: int_data(data)
		{
		}

		[[nodiscard]] const char* getTypeName() const noexcept override
		{
			return "int";
		}

		[[nodiscard]] std::string toString() const override
		{
			return std::to_string(int_data);
		}

		[[nodiscard]] std::vector<drAdaptor> getAdaptors() const final
		{
			auto adaptors = drData::getAdaptors();
			adaptors.emplace_back(drAdaptor{ "binary", &adaptor_toBinary });
			adaptors.emplace_back(drAdaptor{ "hex", &adaptor_toHex });
			return adaptors;
		}

	private:
		static UniquePtr<drData> adaptor_toBinary(const drData& data, const Capture&)
		{
			return soup::make_unique<drString>(string::binary(static_cast<const drInt&>(data).int_data));
		}

		static UniquePtr<drData> adaptor_toHex(const drData& data, const Capture&)
		{
			return soup::make_unique<drString>(string::hex(static_cast<const drInt&>(data).int_data));
		}
	};
}
