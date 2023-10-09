#pragma once

#include "base.hpp"
#if SOUP_WINDOWS

#include <dinput.h>

#include "keys.hpp"

namespace soup
{
	struct DigitalKeyboard
	{
		LPDIRECTINPUT8A pDI = nullptr;
		LPDIRECTINPUTDEVICE8A pKeyboard = nullptr;
		bool keys[NUM_KEYS]{};

		~DigitalKeyboard() noexcept;

		void update();
	};
}

#endif
