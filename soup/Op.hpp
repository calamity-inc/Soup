#pragma once

#include <vector>

namespace soup
{
	struct Op
	{
		uint8_t type = 0xFF;
		std::vector<UniquePtr<ast::Node>> args{};
	};
}
