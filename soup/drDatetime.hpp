#pragma once

#include "drString.hpp"

#include "time.hpp"

namespace soup
{
	struct drDatetime final : public drString
	{
		Datetime datetime_data;

		drDatetime(std::string&& string_data, Datetime&& datetime_data)
			: drString(std::move(string_data)), datetime_data(std::move(datetime_data))
		{
		}

		[[nodiscard]] const char* getTypeName() const noexcept final
		{
			return "datetime";
		}

		[[nodiscard]] std::vector<drAdaptor> getAdaptors() const final;
	};
}
