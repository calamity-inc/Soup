#pragma once

#include "base.hpp"

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

NAMESPACE_SOUP
{
	class Lz77Compressor
	{
	protected:
		static constexpr uint32_t WINDOW = 32768; // max deflate distance
		static constexpr uint32_t MIN_MATCH = 3;
		static constexpr uint32_t MAX_MATCH = 258; // max deflate length
		static constexpr uint32_t HASH_BITS = 15;
		static constexpr uint32_t NIL = 0xffffffffu;

		std::vector<uint32_t> head;
		std::vector<uint32_t> prev; // ring buffer, indexed by pos & (WINDOW-1)
		uint32_t max_chain; // lower = faster, higher = better ratio
		uint32_t nice_len; // stop searching / skip lazy eval when a match is this long
		uint32_t too_far; // drop length-3 matches farther than this

	public:
		Lz77Compressor(uint_fast8_t level/*1..9*/ = 6) SOUP_EXCAL;

		[[nodiscard]] static uint32_t hash(const uint8_t p[3]) noexcept;

		void insert(const uint8_t* data, uint32_t size, uint32_t i) noexcept;

		// Finds the longest match for position i among previously inserted positions.
		// Matches may overlap position i (dist < len), which deflate allows.
		struct Match { uint32_t len = 0, dist = 0; };
		[[nodiscard]] Match findMatch(const uint8_t* data, uint32_t size, uint32_t i) const noexcept;

		[[nodiscard]] std::string compress(const uint8_t* data, size_t size, size_t i = 0);
	};
}
