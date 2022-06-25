#pragma once

#include "Mixed.hpp"

namespace soup
{
	struct Token
	{
		enum ReservedId : int
		{
			_BUILTIN_START = -5009,

			VAL = _BUILTIN_START,
			LITERAL,
			SPACE,

			_BUILTIN_END
		};

		int id;
		Mixed val;

		[[nodiscard]] constexpr bool isBuiltin() const noexcept
		{
			return id >= _BUILTIN_START && id < _BUILTIN_END;
		}
	};
}
