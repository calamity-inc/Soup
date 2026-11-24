#include "Lz77Compressor.hpp"

#include <algorithm> // std::min

#include "deflate.hpp" // encodeLengthDistancePair
#include "macros.hpp" // COUNT
#include "StringWriter.hpp"
#include "utility.hpp" // SOUP_MOVE_RETURN

NAMESPACE_SOUP
{
	struct Config { uint16_t max_chain, nice_len, too_far; };
	static constexpr Config table[9] = {
		//  chain  nice  too_far
		{      4,    8,   2048 }, // 1
		{      8,   16,   2048 }, // 2
		{     32,   32,   4096 }, // 3
		{     16,   16,   4096 }, // 4
		{     32,   32,   4096 }, // 5
		{    128,  128,   4096 }, // 6 (zlib's default)
		{    256,  128,   4096 }, // 7
		{   1024,  258,   4096 }, // 8
		{   4096,  258,   4096 }, // 9
	};

	Lz77Compressor::Lz77Compressor(uint_fast8_t level) SOUP_EXCAL
		: head(size_t(1) << HASH_BITS, NIL), prev(/*size < WINDOW ? size :*/ WINDOW, NIL)
	{
		--level;
		SOUP_IF_UNLIKELY (level >= COUNT(table))
		{
			level = COUNT(table) - 1;
		}
		const Config& c = table[level];
		max_chain = c.max_chain;
		nice_len = c.nice_len;
		too_far = c.too_far;
	}

	uint32_t Lz77Compressor::hash(const uint8_t p[3]) noexcept
	{
		const uint32_t v = p[0] | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16);
		return (v * 0x9E3779B1u) >> (32 - HASH_BITS);
	}

	void Lz77Compressor::insert(const uint8_t* data, uint32_t size, uint32_t i) noexcept
	{
		SOUP_IF_LIKELY (i + MIN_MATCH <= size)
		{
			const uint32_t h = hash(data + i);
			prev[i & (WINDOW - 1)] = head[h];
			head[h] = static_cast<uint32_t>(i);
		}
	}

	// Finds the longest match for position i among previously inserted positions.
	// Matches may overlap position i (dist < len), which deflate allows.
	Lz77Compressor::Match Lz77Compressor::findMatch(const uint8_t* data, uint32_t size, uint32_t i) const noexcept
	{
		Match best;
		SOUP_IF_LIKELY (i + MIN_MATCH <= size)
		{
			const auto limit = std::min<uint32_t>(MAX_MATCH, size - i);
			uint32_t cand = head[hash(data + i)];
			auto chain = max_chain;

			while (cand != NIL && chain-- > 0)
			{
				const auto dist = i - cand;
				if (dist > WINDOW)
				{
					break; // chain is ordered newest->oldest, so all further are too far
				}

				// Quick reject: check the byte that would make this match longer.
				// best.len < limit always holds here, so these reads are data bounds
				if (data[cand + best.len] == data[i + best.len])
				{
					uint32_t len = 0;
					while (len < limit && data[cand + len] == data[i + len])
					{
						++len;
					}
					if (len > best.len)
					{
						best.len = len;
						best.dist = dist;
						if (len >= nice_len || len == limit)
						{
							break;
						}
					}
				}

				const auto next = prev[cand & (WINDOW - 1)];
				if (next >= cand) break; // end of chain (NIL) or stale ring entry
				cand = next;
			}

			SOUP_IF_UNLIKELY (best.len < MIN_MATCH
				|| (best.len == MIN_MATCH && best.dist > too_far) // 3 literals are cheaper with deflate's huffman codes than a far away match
				)
			{
				best = Match{};
			}
		}
		return best;
	}

	std::string Lz77Compressor::compress(const uint8_t* data, size_t _size, size_t _i)
	{
		SOUP_ASSERT(_size < NIL);
		const auto size = static_cast<uint32_t>(_size);
		auto i = static_cast<uint32_t>(_i);

		Match cur = this->findMatch(data, size, i);
		StringWriter sw;
		while (i != size)
		{
			this->insert(data, size, i);

			// "Lazy matching" (if starting one byte later gives a longer match, emit a literal now)
			Match next;
			if (cur.len < this->nice_len && i + 1 < size)
			{
				next = this->findMatch(data, size, i + 1);
			}
			if (cur.len == 0 || next.len > cur.len)
			{
				//std::cout << data[i] << "\n";
				sw.u64_dyn_bp(data[i]);
				++i;
				cur = next; // reuse the lookahead result
				continue;
			}

			//std::cout << "<" << cur.len << ", " << cur.dist << ">\n";
			sw.u64_dyn_bp(deflate::encodeLengthDistancePair(cur.len, cur.dist));
			for (uint32_t k = 1; k != cur.len; ++k)
			{
				this->insert(data, size, i + k);
			}
			i += cur.len;
			cur = (i < size) ? this->findMatch(data, size, i) : Match{};
		}
		SOUP_MOVE_RETURN(sw.data);
	}
}
