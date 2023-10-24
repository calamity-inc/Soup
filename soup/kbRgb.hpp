#pragma once

#include <vector>

#include "Key.hpp"
#include "Rgb.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct kbRgb
	{
		const char* name;

		kbRgb(const char* name)
			: name(name)
		{
		}

		[[nodiscard]] static std::vector<UniquePtr<kbRgb>> getAll(bool include_razer_chroma = true, bool include_no_permission = false);

		virtual ~kbRgb() = default;
	
		void init() {} // Init is implicit when changing colours, but deinit should explicitly be called when you're done.
		virtual void deinit() = 0;

		virtual void setKey(Key key, Rgb colour) = 0;
		virtual void setKeys(const Rgb(&colours)[NUM_KEYS]) = 0;
		virtual void setAllKeys(Rgb colour);

		[[nodiscard]] uint8_t getNumRows() const noexcept { return 6; }
		[[nodiscard]] virtual uint8_t getNumColumns() const noexcept = 0;
		[[nodiscard]] virtual Key mapPosToKey(uint8_t row, uint8_t column) const noexcept = 0;
		void mapPosToKeys(Rgb(&keys)[NUM_KEYS], Rgb* data, uint8_t rows, uint8_t columns);
		[[nodiscard]] std::pair<uint8_t, uint8_t> mapKeyToPos(Key key) const noexcept;
	};
}
