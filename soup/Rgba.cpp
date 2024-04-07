#include "Rgba.hpp"

NAMESPACE_SOUP
{
	Rgba Rgba::fromHex(std::string hex)
	{
		Rgb::expandHex(hex);
		static_assert(sizeof(unsigned long) >= sizeof(uint32_t));
		uint32_t rgba = std::stoul(hex, nullptr, 16);
		if (6 >= hex.size())
		{
			rgba <<= 8;
			rgba |= 0xFF;
		}
		return rgba;
	}
}
