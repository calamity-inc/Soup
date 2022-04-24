#pragma once

#include "MimeMessage.hpp"

namespace soup
{
	struct HttpResponse : public MimeMessage
	{
		using MimeMessage::MimeMessage;

		//std::string status_code;
	};
}
