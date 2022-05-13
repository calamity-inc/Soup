#include "joaat.hpp"

namespace soup
{
	uint32_t joaat::hash(const std::string& str, uint32_t initial) noexcept
	{
		uint32_t val = partial(str.data(), str.size(), initial);
		finalise(val);
		return val;
	}
}
