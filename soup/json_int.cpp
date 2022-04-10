#include "json_int.hpp"

#include "string.hpp"

namespace soup
{
	json_int::json_int(int64_t value) noexcept
		: json_node(JSON_INT), value(value)
	{
	}

	std::string json_int::encode() const
	{
		return string::decimal(value);
	}
}
