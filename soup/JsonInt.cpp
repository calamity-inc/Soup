#include "JsonInt.hpp"

#include "string.hpp"

namespace soup
{
	JsonInt::JsonInt(int64_t value) noexcept
		: JsonNode(JSON_INT), value(value)
	{
	}

	std::string JsonInt::encode() const
	{
		return string::decimal(value);
	}
}
