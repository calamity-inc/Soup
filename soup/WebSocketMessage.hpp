#pragma once

#include <string>

NAMESPACE_SOUP
{
	struct WebSocketMessage
	{
		std::string data;
		bool is_text;
	};
}
