#include "kbRgb.hpp"

#include "kbRgbWooting.hpp"

namespace soup
{
	std::vector<UniquePtr<kbRgb>> kbRgb::getAll()
	{
		std::vector<UniquePtr<kbRgb>> res{};

		for (auto& hid : UsbHid::getAll())
		{
			if (hid.usage_page == 0x1337
				&& hid.vendor_id == 0x31E3
				&& (hid.product_id & 0xFFF0) == 0x1220 // Wooting Two HE
				)
			{
				res.emplace_back(soup::make_unique<kbRgbWooting>(std::move(hid)));
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
}
