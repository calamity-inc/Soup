#pragma once

#include "SharedLibrary.hpp"
#include "UsbHidScancode.hpp"

namespace soup
{
	// In case you don't know, Wooting keyboards are fully analog, so instead of "is key pressed" you can get a percentage how far the key is pressed.
	// For this interface to work, the user needs the Wooting Analog SDK installed on their system.
	// Note that the Wooting Analog SDK will print stuff to the console with no way to disable it. Might just be a Rust thing.
	// ^ Setting the RUST_LOG environment variable to "off" should disable it.
	class WootingAnalog
	{
	private:
		using WootingAnalogResult = int;

		using wooting_analog_initialise_t = WootingAnalogResult(*)();
		using wooting_analog_uninitialise_t = WootingAnalogResult(*)();
		using wooting_analog_read_analog_t = float(*)(unsigned short code);

		SharedLibrary sdk;
		wooting_analog_read_analog_t read_analog;

	public:
		WootingAnalog();
		~WootingAnalog();

		[[nodiscard]] bool isInitialised() const noexcept;

		[[nodiscard]] float read(UsbHidScancode key);
	};
}
