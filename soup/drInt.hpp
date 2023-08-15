#pragma once

#include "drData.hpp"

namespace soup
{
	struct drInt : public drData
	{
		int64_t int_data;

		drInt(int data)
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
	};
}
