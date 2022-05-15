#include "joaat.hpp"

namespace soup
{
	uint32_t joaat::hash(const std::string& str, uint32_t initial) noexcept
	{
		uint32_t val = partial(str.data(), str.size(), initial);
		finalise(val);
		return val;
	}

	[[nodiscard]] static char joaat_find_last_char(uint32_t val)
	{
		char best_c = 0;
		uint32_t best_c_score = -1;
		for (uint8_t c = 0; c != 0x80; ++c)
		{
			uint32_t score = joaat::undo_partial(val, c);
			if (score < best_c_score)
			{
				best_c = c;
				best_c_score = score;
			}
		}
		return best_c;
	}

	std::optional<std::string> joaat::reverse_short_key(uint32_t val)
	{
		undo_finalise(val);
		undo_partial(val);

		std::string str{};
		if (val == 0)
		{
			return str;
		}
		for (uint8_t i = 0; i != 3; ++i)
		{
			char c = joaat_find_last_char(val);
			str.insert(0, 1, c);
			val = undo_partial(val, c);
			if (val == 0)
			{
				return str;
			}
		}

		return {};
	}
}
