#pragma once

#include "base.hpp"
#if (SOUP_WINDOWS || SOUP_LINUX) && !SOUP_CROSS_COMPILE

#if SOUP_WINDOWS
	#ifndef DIRECTINPUT_VERSION
		#define DIRECTINPUT_VERSION 0x0800
	#endif
	#include <dinput.h>
#else
	#include "hwHid.hpp"
#endif

#include "Key.hpp"

NAMESPACE_SOUP
{
	struct DigitalKeyboard
	{
#if SOUP_WINDOWS
		LPDIRECTINPUT8A pDI = nullptr;
		LPDIRECTINPUTDEVICE8A pKeyboard = nullptr;
#else
		hwHid hid;
#endif
		bool keys[NUM_KEYS]{};

		~DigitalKeyboard() noexcept
		{
			deinit();
		}

		void update() noexcept;

		[[nodiscard]] bool isActive() const noexcept
		{
#if SOUP_WINDOWS
			return true;
#else
			return hid.isValid();
#endif
		}

		void deinit() noexcept;
	};
}

#endif
