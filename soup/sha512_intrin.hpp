// THIS FILE IS FOR INTERNAL USE ONLY. DO NOT INCLUDE THIS IN YOUR OWN CODE.

#include "base.hpp"

#include <cstddef>
#include <cstdint>

#if SOUP_BITS == 64 && SOUP_ARM
#include <arm_neon.h>

NAMESPACE_SOUP
{
	namespace intrin
	{
		// Requires FEAT_SHA512 (which FEAT_SHA3 implies). Only call when the CPU reports support.
		#define SOUP_SHA512_INTRIN_TARGET __attribute__((target("arch=armv8.2-a+sha3")))

		// Two SHA-512 rounds. s[0..3] hold the working state as pairs: {a,b}, {c,d}, {e,f}, {g,h}.
		// The register roles rotate every call, so the caller cycles through five role patterns.
	#if defined(__GNUC__) || defined(__clang__)
		__attribute__((target("arch=armv8.2-a+sha3")))
	#endif
		static SOUP_FORCEINLINE void sha512_round_pair(uint64x2_t* s, uint64x2_t wk, int i0, int i1, int i2, int i3, int i4) noexcept
		{
			const uint64x2_t wk_swapped = vextq_u64(wk, wk, 1);
			const uint64x2_t y = vextq_u64(s[i2], s[i3], 1);
			const uint64x2_t z = vextq_u64(s[i1], s[i2], 1);

			uint64x2_t x = vaddq_u64(s[i3], wk_swapped);
			x = vsha512hq_u64(x, y, z);
			s[i4] = vaddq_u64(s[i1], x);
			s[i3] = vsha512h2q_u64(x, s[i1], s[i0]);
		}

	#if defined(__GNUC__) || defined(__clang__)
		__attribute__((target("arch=armv8.2-a+sha3")))
	#endif
		static SOUP_FORCEINLINE uint64x2_t sha512_load_wk(const uint64x2_t* w, const uint64_t* k, size_t pair) noexcept
		{
			return vaddq_u64(w[pair], vld1q_u64(k + pair * 2));
		}

		// state: 8 words, data: one 128-byte block, k: 80 round constants.
	#if defined(__GNUC__) || defined(__clang__)
		__attribute__((target("arch=armv8.2-a+sha3")))
	#endif
		void sha512_transform(uint64_t state[8], const uint8_t data[128], const uint64_t k[80]) noexcept
		{
			// Message schedule: 40 pairs of 64-bit words.
			uint64x2_t w[40];
			for (size_t i = 0; i != 8; ++i)
			{
				// Big-endian words: reverse the bytes within each 64-bit lane.
				w[i] = vreinterpretq_u64_u8(vrev64q_u8(vld1q_u8(data + i * 16)));
			}
			for (size_t i = 8; i != 40; ++i)
			{
				w[i] = vsha512su1q_u64(
					vsha512su0q_u64(w[i - 8], w[i - 7]),
					w[i - 1],
					vextq_u64(w[i - 4], w[i - 3], 1)
				);
			}

			const uint64x2_t init0 = vld1q_u64(&state[0]);
			const uint64x2_t init1 = vld1q_u64(&state[2]);
			const uint64x2_t init2 = vld1q_u64(&state[4]);
			const uint64x2_t init3 = vld1q_u64(&state[6]);

			uint64x2_t s[5];
			s[0] = init0;
			s[1] = init1;
			s[2] = init2;
			s[3] = init3;

			// 40 round-pair calls. The role pattern repeats every 5 calls, so after 40 calls
			// the working state is back in slots 0..3.
			for (size_t q = 0; q != 40; q += 5)
			{
				sha512_round_pair(s, sha512_load_wk(w, k, q + 0), 0, 1, 2, 3, 4);
				sha512_round_pair(s, sha512_load_wk(w, k, q + 1), 3, 0, 4, 2, 1);
				sha512_round_pair(s, sha512_load_wk(w, k, q + 2), 2, 3, 1, 4, 0);
				sha512_round_pair(s, sha512_load_wk(w, k, q + 3), 4, 2, 0, 1, 3);
				sha512_round_pair(s, sha512_load_wk(w, k, q + 4), 1, 4, 3, 0, 2);
			}

			vst1q_u64(&state[0], vaddq_u64(s[0], init0));
			vst1q_u64(&state[2], vaddq_u64(s[1], init1));
			vst1q_u64(&state[4], vaddq_u64(s[2], init2));
			vst1q_u64(&state[6], vaddq_u64(s[3], init3));
		}
	}
}

#endif
