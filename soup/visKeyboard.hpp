#pragma once

#include <cstring> // memset

#include "fwd.hpp"

#include "keys.hpp"

namespace soup
{
	class visKeyboard
	{
	public:
		uint8_t values[NUM_KEYS];
		bool has_ctx_key = false;

		visKeyboard() noexcept
		{
			clear();
		}

		void clear() noexcept
		{
			memset(values, 0, sizeof(values));
		}

		// 195 x 50 per scale
		void draw(RenderTarget& rt, unsigned int x, unsigned int y, uint8_t scale = 1) const;
	protected:
		void drawKey(RenderTarget& rt, uint8_t scale, const char* label, uint8_t value, unsigned int x, unsigned int y, unsigned int width = 10, unsigned int height = 10) const;
	};
}
