#pragma once

#include <cstdint>

namespace soup
{
	enum buttons : uint8_t
	{
		BTN_DPAD_UP = 0,
		BTN_DPAD_DOWN,
		BTN_DPAD_LEFT,
		BTN_DPAD_RIGHT,

		               // Standard  PS   Switch
		BTN_ACT_DOWN,  // (A)       (X)  (B)
		BTN_ACT_RIGHT, // (B)       (O)  (A)
		BTN_ACT_LEFT,  // (X)       (◻)  (Y)
		BTN_ACT_UP,    // (Y)       (△)  (X)

		BTN_LBUMPER,
		BTN_RBUMPER,
		BTN_LTRIGGER,
		BTN_RTRIGGER,
		BTN_SHARE,
		BTN_OPTIONS,
		BTN_LSTICK,
		BTN_RSTICK,

		BTN_META,
		BTN_TOUCHPAD,

		NUM_BUTTONS
	};
}
