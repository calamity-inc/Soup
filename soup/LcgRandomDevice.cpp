#include "LcgRandomDevice.hpp"

#include "intutil.hpp"
#include "time.hpp"

namespace soup
{
	LcgRandomDevice::LcgRandomDevice()
		: LcgRandomDevice(~time::nanos())
	{
	}

	uint64_t LcgRandomDevice::generate()
	{
		last *= multiplier;
		last += increment;

		// invert byte order since the higher-order bits have longer periods
		return intutil::invertEndianness(last);
	}
}
