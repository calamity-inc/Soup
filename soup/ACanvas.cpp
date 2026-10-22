#include "ACanvas.hpp"

#include "Endian.hpp"

NAMESPACE_SOUP
{
	std::string ACanvas::toPam() const
	{
		std::string str = "P7\nWIDTH ";
		str.append(std::to_string(width));
		str.append("\nHEIGHT ").append(std::to_string(height));
		str.append("\nDEPTH 4\nMAXVAL 255\nTUPLTYPE RGB_ALPHA\nENDHDR\n");
		str.reserve(str.size() + (width * height * 4));
		for (const auto& p : pixels)
		{
			const uint32_t val = Endianness::toNetwork(p.toInt());
			str.append((const char*)&val, 4);
		}
		return str;
	}
}
