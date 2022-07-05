#include "MtRandomDevice.hpp"

#include "rand.hpp"

namespace soup
{
	uint64_t MtRandomDevice::generate()
	{
		return rand.t<uint64_t>(0, -1);
	}
}
