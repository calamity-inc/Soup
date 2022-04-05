#pragma once

#include "mime_message.hpp"

namespace soup
{
	struct http_response : public mime_message
	{
		using mime_message::mime_message;

		//std::string status_code;
	};
}
