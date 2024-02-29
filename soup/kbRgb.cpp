#include "kbRgb.hpp"

#include "base.hpp"
#if !SOUP_WASM

#include "json.hpp"
#include "kbRgbCorsairCue.hpp"
#include "kbRgbRazerChroma.hpp"
#include "kbRgbWooting.hpp"

namespace soup
{
	std::vector<UniquePtr<kbRgb>> kbRgb::getAll(bool include_razer_chroma, bool include_no_permission)
	{
		std::vector<UniquePtr<kbRgb>> res{};

		if (include_razer_chroma
			&& kbRgbRazerChroma::isAvailable()
			)
		{
			res.emplace_back(soup::make_unique<kbRgbRazerChroma>());
		}

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
						res.emplace_back(soup::make_unique<kbRgbWooting>("Wooting One", 17, false, std::move(hid)));
					}
					else if ((hid.product_id & 0xFFF0) == 0x1200)
					{
						res.emplace_back(soup::make_unique<kbRgbWooting>("Wooting Two", 21, false, std::move(hid)));
					}
					else if ((hid.product_id & 0xFFF0) == 0x1210)
					{
						res.emplace_back(soup::make_unique<kbRgbWooting>("Wooting Lekker", 21, false, std::move(hid)));
					}
					else if ((hid.product_id & 0xFFF0) == 0x1220)
					{
						res.emplace_back(soup::make_unique<kbRgbWooting>("Wooting Two HE", 21, false, std::move(hid)));
					}
					else if ((hid.product_id & 0xFFF0) == 0x1230)
					{
						res.emplace_back(soup::make_unique<kbRgbWooting>("Wooting Two HE ARM", 21, true, std::move(hid)));
					}
					else if ((hid.product_id & 0xFFF0) == 0x1300)
					{
						res.emplace_back(soup::make_unique<kbRgbWooting>("Wooting 60HE", 14, false, std::move(hid)));
					}
					else if ((hid.product_id & 0xFFF0) == 0x1310)
					{
						res.emplace_back(soup::make_unique<kbRgbWooting>("Wooting 60HE ARM", 14, true, std::move(hid)));
					}
					else if ((hid.product_id & 0xFFF0) == 0x1320)
					{
						res.emplace_back(soup::make_unique<kbRgbWooting>("Wooting 60HE+", 14, true, std::move(hid)));
					}
					else if ((hid.product_id & 0xFFF0) == 0x1510)
					{
						res.emplace_back(soup::make_unique<kbRgbWooting>("Wooting UwU RGB", 7, true, std::move(hid)));
					}
				}
			}
		}

#if SOUP_WINDOWS
		if (kbRgbCorsairCue::ensureConnected())
		{
			kbRgbCorsairCue::send(R"({"method":1,"params":{"deviceTypeMask":1,"sizeMax":10},"code":2})");
			// For some users, 'kbRgbCorsairCue::receive())' apparently returns an empty string. Handle it somewhat gracefully.
			if (auto jr = json::decode(kbRgbCorsairCue::receive()))
			{
				for (const auto& dev : jr->asObj().at("result").asObj().at("deviceInfos").asArr())
				{
					res.emplace_back(soup::make_unique<kbRgbCorsairCue>(
						std::move(dev.asObj().at("id").asStr().value),
						std::move(dev.asObj().at("model").asStr().value)
					));
				}
			}
		}
#endif

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
				if (auto sk = mapPosToKey(row, column); sk != KEY_NONE)
				{
					keys[sk] = data[row * columns + column];
				}
			}
		}
	}

	std::vector<std::pair<uint8_t, uint8_t>> kbRgb::mapKeyToPos(Key key) const noexcept
	{
		std::vector<std::pair<uint8_t, uint8_t>> res;
		const uint8_t rows = getNumRows();
		const uint8_t columns = getNumColumns();
		for (uint8_t row = 0; row != rows; ++row)
		{
			for (uint8_t column = 0; column != columns; ++column)
			{
				if (mapPosToKey(row, column) == key)
				{
					res.emplace_back(row, column);
					if (key != KEY_BACKSLASH || res.size() == 2)
					{
						return res;
					}
				}
			}
		}
		return res;
	}

	bool kbRgb::isWooting() const noexcept
	{
		return name.length() > 8 && name.substr(0, 8) == "Wooting ";
	}
}
#endif
