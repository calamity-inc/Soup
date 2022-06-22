#pragma once

#include "Mixed.hpp"

namespace soup
{
	struct Token
	{
		enum ReservedId : int
		{
			VAL = -1,
		};

		int id;
		Mixed val;
	};
}
