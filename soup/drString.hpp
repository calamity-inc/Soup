#pragma once

#include "drData.hpp"

NAMESPACE_SOUP
{
	struct drString : public drData
	{
		std::string string_data;

		drString(std::string&& data)
			: string_data(std::move(data))
		{
		}

		[[nodiscard]] static UniquePtr<drString> reflect(std::string str);

		[[nodiscard]] const char* getTypeName() const noexcept override
		{
			return "string";
		}

		[[nodiscard]] std::string toString() const override
		{
			return string_data;
		}
	};
}
