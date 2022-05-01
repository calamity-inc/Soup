#pragma once

#include <string>

namespace soup
{
	struct WebSocketMessage
	{
		std::string data;
		bool is_text;
	};
}
