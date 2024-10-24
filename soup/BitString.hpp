#pragma once

#include "BitWriter.hpp"
#include "StringWriter.hpp"

NAMESPACE_SOUP
{
	class BitString
	{
	public:
		StringWriter sw;
		BitWriter bw;

		explicit BitString()
			: sw(), bw(&sw)
		{
		}

		explicit BitString(const BitString& b)
			: sw(b.sw), bw(b.bw)
		{
			associate();
		}

		explicit BitString(BitString&& b)
			: sw(std::move(b.sw)), bw(std::move(b.bw))
		{
			associate();
		}

		void operator=(BitString&& b)
		{
			sw.data = std::move(b.sw.data);
			bw = std::move(b.bw);
			associate();
		}

	protected:
		void associate() noexcept
		{
			bw.w = &sw;
		}

	public:
		[[nodiscard]] size_t getBitLength() const noexcept
		{
			return (sw.data.size() * 8) + bw.bit_idx;
		}

		[[nodiscard]] BitWriter& operator*() noexcept
		{
			return bw;
		}

		[[nodiscard]] BitWriter* operator->() noexcept
		{
			return &bw;
		}

		void commit(BitWriter& target) const
		{
			SOUP_ASSERT(target.isByteAligned()); // no idea why, but this just breaks big time if the target is not byte-aligned
			for (const auto& c : sw.data)
			{
				target.u8(8, (uint8_t)c);
			}
			target.u8(bw.bit_idx, bw.byte);
		}
	};
}
