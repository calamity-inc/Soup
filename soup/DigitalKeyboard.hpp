#pragma once

#include "base.hpp"
#if SOUP_WINDOWS

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>

#include "Key.hpp"

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
