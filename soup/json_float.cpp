#include "json_float.hpp"

#include <string>

namespace soup
{
	json_float::json_float(double value) noexcept
		: json_node(JSON_FLOAT), value(value)
	{
	}

	std::string json_float::encode() const
	{
		return std::to_string(value);
	}
}
