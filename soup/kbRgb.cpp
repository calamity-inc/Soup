#include "kbRgb.hpp"

#include "kbRgbWooting.hpp"

namespace soup
{
	std::vector<UniquePtr<kbRgb>> kbRgb::getAll(bool include_no_permission)
	{
		std::vector<UniquePtr<kbRgb>> res{};

		for (auto& hid : hwHid::getAll())
		{
			if (include_no_permission || hid.havePermission())
			{
				if (hid.usage_page == 0x1337
					&& hid.vendor_id == 0x31E3
					)
				{
					if ((hid.product_id & 0xFFF0) == 0x1100)
					{
						// Untested
						res.emplace_back(soup::make_unique<kbRgbWooting>("Wooting One", false, std::move(hid)));
					}
					else if ((hid.product_id & 0xFFF0) == 0x1200)
					{
						res.emplace_back(soup::make_unique<kbRgbWooting>("Wooting Two", true, std::move(hid)));
					}
					else if ((hid.product_id & 0xFFF0) == 0x1220)
					{
						res.emplace_back(soup::make_unique<kbRgbWooting>("Wooting Two HE", true, std::move(hid)));
					}
				}
			}
		}

		return res;
	}

	void kbRgb::setAllKeys(Rgb colour)
	{
		Rgb buf[NUM_KEYS];
		for (auto& c : buf)
		{
			c = colour;
		}
		setKeys(buf);
	}

	void kbRgb::mapPosToKeys(Rgb(&keys)[NUM_KEYS], Rgb* data, uint8_t rows, uint8_t columns)
	{
		for (uint8_t row = 0; row != rows; ++row)
		{
			for (uint8_t column = 0; column != columns; ++column)
			{
				if (auto sk = getKeyForPos(row, column); sk != KEY_NONE)
				{
					keys[sk] = data[row * columns + column];
				}
			}
		}
	}
}
