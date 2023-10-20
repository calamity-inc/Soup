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

		BTN_LBUMPER, // aka. L1
		BTN_RBUMPER, // aka. R1
		BTN_LTRIGGER, // aka. L2
		BTN_RTRIGGER, // aka. R2
		BTN_LSTICK, // aka. L3
		BTN_RSTICK, // aka. R3

		BTN_META, // "Home" on Switch
		BTN_TOUCHPAD, // DS4
		BTN_SHARE, // "Capture" on Switch & Stadia
		BTN_OPTIONS, // Not on Switch; check BTN_OPTIONS, BTN_MINUS & BTN_PLUS for "Escape Key" equivalent use cases.
		BTN_MINUS, // Switch 
		BTN_PLUS, // Switch
		BTN_ASSISTANT, // Stadia
		BTN_MENU, // Stadia

		NUM_BUTTONS
	};
}
