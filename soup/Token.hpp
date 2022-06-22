#pragma once

#include <string>

namespace soup
{
	struct Token
	{
		enum ReservedId : int
		{
			STRING = -1,
		};

		int id;
		std::string val{};
	};
}
