#pragma once

#include "Mixed.hpp"

namespace soup
{
	struct Lexeme
	{
		enum ReservedId : int
		{
			_BUILTIN_START = -5009,

			VAL = _BUILTIN_START,
			LITERAL,
			SPACE,

			_BUILTIN_END
		};

		int type;
		Mixed val;

		[[nodiscard]] constexpr bool isBuiltin() const noexcept
		{
			return type >= _BUILTIN_START && type < _BUILTIN_END;
		}
	};
}
