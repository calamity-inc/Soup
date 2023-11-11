#include "sha256.hpp"

#include "base.hpp"
#include "sha_commons.hpp"

#if SOUP_X86 && SOUP_BITS == 64 && defined(SOUP_USE_INTRIN)
#define SHA256_USE_INTRIN true
#else
#define SHA256_USE_INTRIN false
#endif

#include <cstring> // memcpy

#include "StringRefReader.hpp"

#if SHA256_USE_INTRIN
#include "CpuInfo.hpp"
#include "Endian.hpp"
#endif

namespace soup
{
	// Adapted from https://github.com/System-Glitch/SHA256

	static constexpr auto BLOCK_INTS = sha256::BLOCK_BYTES / sizeof(uint32_t);

	inline static uint32_t rotr(const uint32_t value, const size_t bits)
	{
		return (value >> bits) | (value << (32 - bits));
	}

	inline static uint32_t choose(uint32_t e, uint64_t f, uint64_t g)
	{
		return (e & f) ^ (~static_cast<uint64_t>(e) & g);
	}

	inline static uint32_t majority(uint32_t a, uint64_t b, uint64_t c)
	{
		return static_cast<uint32_t>((a & (b | c)) | (b & c));
	}

	inline static uint32_t sig0(uint32_t x)
	{
		return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
	}

	inline static uint32_t sig1(uint32_t x)
	{
		return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
	}

	static constexpr uint32_t K[64] = {
		0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
		0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
		0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
		0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
		0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
		0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
		0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
		0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
		0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
		0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
		0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
		0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
		0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
		0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
		0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
		0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
	};

	inline static void transform(uint32_t digest[], uint32_t block[BLOCK_INTS])
	{
		uint32_t state[8];
		memcpy(state, digest, sha256::DIGEST_BYTES);

		uint32_t maj, xorA, ch, xorE, sum, newA, newE, m[64];

		// Split data in 32 bit blocks for the 16 first words
		for (uint8_t i = 0; i != 16; ++i)
		{
			m[i] = block[i];
		}

		// Remaining 48 blocks
		for (uint8_t k = 16; k < 64; k++)
		{
			m[k] = sig1(m[k - 2]) + m[k - 7] + sig0(m[k - 15]) + m[k - 16];
		}

		for (uint8_t i = 0; i < 64; i++)
		{
			maj = majority(state[0], state[1], state[2]);
			xorA = rotr(state[0], 2) ^ rotr(state[0], 13) ^ rotr(state[0], 22);

			ch = choose(state[4], state[5], state[6]);

			xorE = rotr(state[4], 6) ^ rotr(state[4], 11) ^ rotr(state[4], 25);

			sum = m[i] + K[i] + state[7] + ch + xorE;
			newA = xorA + maj + sum;
			newE = state[3] + sum;

			state[7] = state[6];
			state[6] = state[5];
			state[5] = state[4];
			state[4] = newE;
			state[3] = state[2];
			state[2] = state[1];
			state[1] = state[0];
			state[0] = newA;
		}

		for (uint8_t i = 0; i < 8; i++)
		{
			digest[i] += state[i];
		}
	}

	std::string sha256::hash(const std::string& str)
	{
		StringRefReader r(str);
		return hash(r);
	}

#if SHA256_USE_INTRIN
	extern void sha256_transform_intrin(uint32_t state[8], const uint8_t data[]);
#endif

	template <bool intrin>
	static std::string sha256_hash_impl(ioSeekableReader& r)
	{
		// init
		uint32_t digest[] = {
			0x6a09e667,
			0xbb67ae85,
			0x3c6ef372,
			0xa54ff53a,
			0x510e527f,
			0x9b05688c,
			0x1f83d9ab,
			0x5be0cd19,
		};
		static_assert(sizeof(digest) == sha256::DIGEST_BYTES);

		std::string buffer{};
		uint64_t transforms = 0;

		size_t in_len = r.getRemainingBytes();

		// update
		for (; in_len >= sha256::BLOCK_BYTES; in_len -= sha256::BLOCK_BYTES)
		{
			r.str(sha256::BLOCK_BYTES, buffer);
#if SHA256_USE_INTRIN
			if constexpr (intrin)
			{
				sha256_transform_intrin(digest, (const uint8_t*)buffer.data());
			}
			else
#endif
			{
				uint32_t block[BLOCK_INTS];
				buffer_to_block<BLOCK_INTS, intrin>(buffer, block);
				transform(digest, block);
			}
			buffer.clear();
			++transforms;
		}

		r.str(in_len, buffer);

		// final

		/* Total number of hashed bits */
		uint64_t total_bits = (transforms * sha256::BLOCK_BYTES + buffer.size()) * 8;

		/* Padding */
		buffer += (char)0x80;
		size_t orig_size = buffer.size();
		while (buffer.size() < sha256::BLOCK_BYTES)
		{
			buffer += (char)0x00;
		}

		uint32_t block[BLOCK_INTS];
		buffer_to_block<BLOCK_INTS, intrin>(buffer, block);

		if (orig_size > sha256::BLOCK_BYTES - 8)
		{
#if SHA256_USE_INTRIN
			if constexpr (intrin)
			{
				sha256_transform_intrin(digest, (const uint8_t*)buffer.data());
			}
			else
#endif
			{
				transform(digest, block);
			}
			for (size_t i = 0; i < BLOCK_INTS - 2; i++)
			{
				block[i] = 0;
			}
		}

		/* Append total_bits, split this uint64_t into two uint32_t */
#if SHA256_USE_INTRIN
		if constexpr (intrin)
		{
			block[BLOCK_INTS - 1] = Endianness::invert((uint32_t)total_bits);
			block[BLOCK_INTS - 2] = Endianness::invert((uint32_t)(total_bits >> 32));
			sha256_transform_intrin(digest, (const uint8_t*)block);
		}
		else
#endif
		{
			block[BLOCK_INTS - 1] = (uint32_t)total_bits;
			block[BLOCK_INTS - 2] = (uint32_t)(total_bits >> 32);
			transform(digest, block);
		}

		std::string bin{};
		bin.reserve(sha256::DIGEST_BYTES);
		for (size_t i = 0; i < sizeof(digest) / sizeof(digest[0]); i++)
		{
			bin.push_back(((const char*)&digest[i])[3]);
			bin.push_back(((const char*)&digest[i])[2]);
			bin.push_back(((const char*)&digest[i])[1]);
			bin.push_back(((const char*)&digest[i])[0]);
		}
		return bin;
	}

	std::string sha256::hash(ioSeekableReader& r)
	{
#if SHA256_USE_INTRIN
		const CpuInfo& cpu_info = CpuInfo::get();
		if (cpu_info.supportsSSSE3()
			&& cpu_info.supportsSHA()
			)
		{
			return sha256_hash_impl<true>(r);
		}
#endif
		return sha256_hash_impl<false>(r);
	}
}
