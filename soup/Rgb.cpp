#include "Rgb.hpp"

#include "string.hpp"

namespace soup
{
	Rgb Rgb::BLACK = { 0, 0, 0 };
	Rgb Rgb::WHITE = { 255, 255, 255 };

	bool Rgb::operator==(const Rgb& c) const noexcept
	{
		return r == c.r && g == c.g && b == c.b;
	}

	bool Rgb::operator!=(const Rgb& c) const noexcept
	{
		return !operator==(c);
	}

	std::string Rgb::toHex() const
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
