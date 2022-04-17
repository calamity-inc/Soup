#include "rgb.hpp"

#include "string.hpp"

namespace soup
{
	rgb rgb::BLACK = { 0, 0, 0 };
	rgb rgb::WHITE = { 255, 255, 255 };

	bool rgb::operator==(const rgb& c) const noexcept
	{
		return r == c.r && g == c.g && b == c.b;
	}

	bool rgb::operator!=(const rgb& c) const noexcept
	{
		return !operator==(c);
	}

	std::string rgb::toHex() const
	{
		if ((r | g | b) & 0xF0)
		{
			std::string str = string::lpad(string::hex(r), 2, '0');
			str.append(string::lpad(string::hex(g), 2, '0'));
			str.append(string::lpad(string::hex(b), 2, '0'));
			return str;
		}
		std::string str = string::hex(r);
		str.append(string::hex(g));
		str.append(string::hex(b));
		return str;
	}
}
