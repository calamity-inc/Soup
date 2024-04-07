#pragma once

#include <cstdint>

#include "base.hpp"

NAMESPACE_SOUP
{
	enum ControlInput : uint8_t
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		NEW_LINE,
		BACKSPACE,
	};
}
