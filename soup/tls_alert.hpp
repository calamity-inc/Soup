#pragma once

#include "packet.hpp"

namespace soup
{
	SOUP_PACKET(tls_alert)
	{
		enum tls_alert_level : uint8_t
		{
			warning = 1,
			fatal = 2,
		};
	};
}
