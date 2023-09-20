#include "Rgb.hpp"

#include "joaat.hpp"
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
	const Rgb Rgb::GREY{ 128, 128, 128 };
	const Rgb Rgb::LIGHTGREY{ 211, 211, 211 };

	Rgb Rgb::fromHex(std::string hex)
	{
		expandHex(hex);
		static_assert(sizeof(unsigned long) >= sizeof(uint32_t));
		return Rgb(std::stoul(hex, nullptr, 16));
	}

	void Rgb::expandHex(std::string& hex)
	{
		if (hex.at(0) == '#')
		{
			hex.erase(0, 1);
		}
		if (hex.size() == 3)
		{
			hex = std::move(std::string(2, hex.at(0)).append(2, hex.at(1)).append(2, hex.at(2)));
		}
	}

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

	std::optional<Rgb> Rgb::fromName(const std::string& name)
	{
		switch (joaat::hash(name))
		{
		case joaat::hash("black"): return BLACK;
		case joaat::hash("white"): return WHITE;
		case joaat::hash("red"): return RED;
		case joaat::hash("yellow"): return YELLOW;
		case joaat::hash("green"): return GREEN;
		case joaat::hash("blue"): return BLUE;
		case joaat::hash("magenta"): return MAGENTA;
		case joaat::hash("grey"): case joaat::hash("gray"): return GREY;
		case joaat::hash("lightgrey"): case joaat::hash("lightgray"): return LIGHTGREY;
		}
		return std::nullopt;
	}
}
