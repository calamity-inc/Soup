#pragma once

#include "scoped_enum.hpp"

namespace soup
{
	SCOPED_ENUM(WebSocketFrameType, uint8_t,
		CONTINUATION = 0,
		TEXT = 1,
		BINARY = 2,
		_NON_CONTROL_MAX = BINARY,

		CLOSE = 8,
		PING = 9,
		PONG = 10,
	);
}
