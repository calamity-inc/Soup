#pragma once

#include "type.hpp"

namespace soup
{
	struct tls_content_type
	{
		enum _ : tls_content_type_t
		{
			change_cipher_spec = 20,
			alert = 21,
			handshake = 22,
			application_data = 23,
		};
	};
}
