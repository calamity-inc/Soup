#include "Rgb.hpp"

#include "string.hpp"

namespace soup
{
	const Rgb Rgb::BLACK{ 0, 0, 0 };
	const Rgb Rgb::WHITE{ 255, 255, 255 };
	const Rgb Rgb::RED{ 255, 0, 0 };
	const Rgb Rgb::YELLOW{ 255, 255, 0 };
	const Rgb Rgb::GREEN{ 0, 255, 0 };
	const Rgb Rgb::BLUE{ 0, 0, 255 };
	const Rgb Rgb::MAGENTA{ 255, 0, 255 };

	std::string Rgb::toHex() const
	{
		if (((r >> 4) == (r & 0xF))
			&& ((g >> 4) == (g & 0xF))
			&& ((b >> 4) == (b & 0xF))
			)
		{
			std::string str = string::hex(r & 0xF);
			str.append(string::hex(g & 0xF));
			str.append(string::hex(b & 0xF));
			return str;
		}
		std::string str = string::lpad(string::hex(r), 2, '0');
		str.append(string::lpad(string::hex(g), 2, '0'));
		str.append(string::lpad(string::hex(b), 2, '0'));
		return str;
	}
}
