#pragma once

#include <cstdint>

#include "base.hpp"

NAMESPACE_SOUP
{
	enum irType : uint8_t
	{
		IR_BOOL,
		IR_I8,
		//IR_I16,
		IR_I32,
		IR_I64,
		IR_PTR,
	};
}
