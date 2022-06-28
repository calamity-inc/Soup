#pragma once

namespace soup
{
	enum BuiltinOp : uint8_t
	{
		OP_PUSH_STR = 0xFF - 0,
		OP_PUSH_INT = 0xFF - 1,
		OP_PUSH_VAR = 0xFF - 2,
		OP_PUSH_FUN = 0xFF - 3,
	};
}
