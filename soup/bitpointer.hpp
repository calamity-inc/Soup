#pragma once

#include "bitset.hpp"

namespace soup
{
	class bitpointer
	{
	private:
		bitset<uint8_t>* bs;
		uint8_t mask;

	public:
		bitpointer(void* _addr, uint8_t bit) noexcept
		{
			auto addr = static_cast<uint8_t*>(_addr);

			addr += (bit / 8);
			bit %= 8;

			bs = bitset<uint8_t>::at(addr);
			mask = bitset<uint8_t>::calculateMask(bit);
		}

		[[nodiscard]] bool get() const noexcept
		{
			return bs->getMask(mask);
		}

		void set(const bool v) noexcept
		{
			bs->setMask(mask, v);
		}

		void enable() noexcept
		{
			bs->enableMask(mask);
		}

		void disable() noexcept
		{
			bs->disableMask(mask);
		}
	};
}
