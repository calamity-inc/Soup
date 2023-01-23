#include "WootingAnalog.hpp"

namespace soup
{
	WootingAnalog::WootingAnalog()
		: sdk("wooting_analog_sdk")
	{
		if (sdk.isLoaded())
		{
			if (((wooting_analog_initialise_t)sdk.getAddress("wooting_analog_initialise"))() >= 0)
			{
				read_analog = (wooting_analog_read_analog_t)sdk.getAddress("wooting_analog_read_analog");
				return;
			}
			sdk.unload();
		}
		read_analog = nullptr;
	}

	WootingAnalog::~WootingAnalog()
	{
		if (isInitialised())
		{
			((wooting_analog_uninitialise_t)sdk.getAddress("wooting_analog_uninitialise"))();
			sdk.unload();
		}
	}

	bool WootingAnalog::isInitialised() const noexcept
	{
		return sdk.isLoaded();
	}

	float WootingAnalog::read(UsbHidScancode key)
	{
		return read_analog(key);
	}
}
