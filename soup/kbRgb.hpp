#pragma once

#include <vector>

#include "fwd.hpp"
#include "Key.hpp"
#include "Rgb.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct kbRgb
	{
		std::string name;

		kbRgb(std::string name)
			: name(std::move(name))
		{
		}

		[[nodiscard]] static std::vector<UniquePtr<kbRgb>> getAll(bool include_razer_chroma, bool include_no_permission = false);

		virtual ~kbRgb() = default;

		[[nodiscard]] virtual bool controlsDevice(const hwHid& hid) const noexcept = 0;

		void init() {} // Init is implicit when changing colours, but deinit should explicitly be called when you're done.
		virtual void deinit() = 0;

		virtual void setKey(Key key, Rgb colour) = 0;
		virtual void setKeys(const Rgb(&colours)[NUM_KEYS]) = 0;
		virtual void setAllKeys(Rgb colour);

		static constexpr uint8_t MAX_ROWS = 6;
		static constexpr uint8_t MAX_COLUMNS = 22;

		[[nodiscard]] uint8_t getNumRows() const noexcept { return 6; }
		[[nodiscard]] virtual uint8_t getNumColumns() const noexcept = 0;
		[[nodiscard]] virtual Key mapPosToKey(uint8_t row, uint8_t column) const noexcept = 0;
		void mapPosToKeys(Rgb(&keys)[NUM_KEYS], Rgb* data, uint8_t rows, uint8_t columns);
		[[nodiscard]] std::vector<std::pair<uint8_t, uint8_t>> mapKeyToPos(Key key) const noexcept;

		[[nodiscard]] bool isWooting() const noexcept;
		[[nodiscard]] kbRgbWooting* asWooting() noexcept { return reinterpret_cast<kbRgbWooting*>(this); }
	};
}
