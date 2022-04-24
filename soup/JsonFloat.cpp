#include "JsonFloat.hpp"

#include <string>

namespace soup
{
	JsonFloat::JsonFloat(double value) noexcept
		: JsonNode(JSON_FLOAT), value(value)
	{
	}

	std::string JsonFloat::encode() const
	{
		return std::to_string(value);
	}
}
