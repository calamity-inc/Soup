#pragma once

#include <vector>

#include "keys.hpp"
#include "Rgb.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct kbRgb
	{
		[[nodiscard]] static std::vector<UniquePtr<kbRgb>> getAll();

		virtual ~kbRgb() = default;

		virtual void init() = 0;
		virtual void deinit() = 0;

		virtual void setKey(uint8_t key, Rgb colour) = 0;
		virtual void setKeys(const Rgb(&colours)[NUM_KEYS]) = 0;
		virtual void setAllKeys(Rgb colour);
	};
}
