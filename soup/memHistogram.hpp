#pragma once

#include <algorithm>

#include "Packet.hpp"
#include "Range.hpp"

NAMESPACE_SOUP
{
	SOUP_PACKET(memHistogram)
	{
		uint64_t data[256]; // byte -> occurrences

		memHistogram() noexcept
		{
			memset(data, 0, sizeof(data));
		}

		void addBytesFrom(const Range& r) noexcept
		{
			size_t i = 0;
#if SOUP_BITS >= 64
			// this is probably better for pipelining than the loop below
			for (; i + 8 <= r.size; i += 8)
			{
				uint64_t v = *r.base.add(i).as<uint64_t*>();
				++data[v & 0xff];
				++data[(v >> 8) & 0xff];
				++data[(v >> 16) & 0xff];
				++data[(v >> 24) & 0xff];
				++data[(v >> 32) & 0xff];
				++data[(v >> 40) & 0xff];
				++data[(v >> 48) & 0xff];
				++data[v >> 56];
			}
#endif
			for (; i != r.size; ++i)
			{
				++data[*r.base.add(i).as<uint8_t*>()];
			}
		}

		void toFrequencies(uint8_t out[256]) const
		{
			std::pair<uint8_t, uint64_t> pairs[256]; // <byte, occurrences>
			for (uint_fast16_t i = 0; i != 256; ++i)
			{
				pairs[i].first = i;
				pairs[i].second = data[i];
			}
			// pairs is now sorted by `byte` ascending but we want it sorted by `occurrences` ascending.
			std::sort(&pairs[0], &pairs[256], [](const std::pair<uint8_t, uint64_t>& a, const std::pair<uint8_t, uint64_t>& b)
			{
				return a.second < b.second;
			});
			for (uint_fast16_t i = 0; i != 256; ++i)
			{
				out[pairs[i].first] = i; // byte -> frequency
			}
		}

		SOUP_PACKET_IO(s)
		{
			bool b = true;
			for (uint64_t& f : data)
			{
				b &= s.u64_le(f);
			}
			return b;
		}

		void operator +=(const memHistogram& b) noexcept
		{
			for (uint_fast16_t i = 0; i != 256; ++i)
			{
				data[i] += b.data[i];
			}
		}
	};
}
