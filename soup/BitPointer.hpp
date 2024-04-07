#pragma once

#include "Bitset.hpp"

NAMESPACE_SOUP
{
	class BitPointer
	{
	private:
		Bitset<uint8_t>* bs;
		uint8_t mask;

	public:
		BitPointer(void* _addr, uint8_t bit) noexcept
		{
			auto addr = static_cast<uint8_t*>(_addr);

			addr += (bit / 8);
			bit %= 8;

			bs = Bitset<uint8_t>::at(addr);
			mask = Bitset<uint8_t>::calculateMask(bit);
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
