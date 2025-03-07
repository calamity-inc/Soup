#include "md5.hpp"

// Original source: https://github.com/Zunawe/md5-c
// Original licence: The Unlicence

NAMESPACE_SOUP
{
	static uint32_t S[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
						   5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
						   4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
						   6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

	static uint32_t K[] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
						   0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
						   0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
						   0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
						   0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
						   0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
						   0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
						   0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
						   0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
						   0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
						   0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
						   0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
						   0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
						   0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
						   0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
						   0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

	static uint8_t PADDING[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	#define F(X, Y, Z) ((X & Y) | (~X & Z))
	#define G(X, Y, Z) ((X & Z) | (Y & ~Z))
	#define H(X, Y, Z) (X ^ Y ^ Z)
	#define I(X, Y, Z) (Y ^ (X | ~Z))

	[[nodiscard]] static uint32_t rotateLeft(uint32_t x, uint32_t n) noexcept
	{
		return (x << n) | (x >> (32 - n));
	}

	static void md5Step(uint32_t state[4], uint32_t block[16]) noexcept
	{
		uint32_t AA = state[0];
		uint32_t BB = state[1];
		uint32_t CC = state[2];
		uint32_t DD = state[3];

		uint32_t E;

		unsigned int j;

		for (unsigned int i = 0; i < 64; ++i)
		{
			switch (i / 16)
			{
				case 0:
					E = F(BB, CC, DD);
					j = i;
					break;
				case 1:
					E = G(BB, CC, DD);
					j = ((i * 5) + 1) % 16;
					break;
				case 2:
					E = H(BB, CC, DD);
					j = ((i * 3) + 5) % 16;
					break;
				default:
					E = I(BB, CC, DD);
					j = (i * 7) % 16;
					break;
			}

			uint32_t temp = DD;
			DD = CC;
			CC = BB;
			BB = BB + rotateLeft(AA + E + K[i] + block[j], S[i]);
			AA = temp;
		}

		state[0] += AA;
		state[1] += BB;
		state[2] += CC;
		state[3] += DD;
	}

	md5::State::State()
	{
		state[0] = (uint32_t)0x67452301;
		state[1] = (uint32_t)0xefcdab89;
		state[2] = (uint32_t)0x98badcfe;
		state[3] = (uint32_t)0x10325476;
		buffer_counter = 0;
		n_bits = 0;
	}

	std::string md5::hash(const void* data, size_t len)
	{
		State state;
		state.append(data, len);
		state.finalise();
		return state.getDigest();
	}

	void md5::State::transform() noexcept
	{
		uint32_t block[16];
		for (unsigned int j = 0; j < 16; ++j)
		{
			block[j] = (uint32_t)(buffer[(j * 4) + 3]) << 24 |
				(uint32_t)(buffer[(j * 4) + 2]) << 16 |
				(uint32_t)(buffer[(j * 4) + 1]) << 8 |
				(uint32_t)(buffer[(j * 4)]);
		}
		md5Step(state, block);
	}

	void md5::State::finalise() noexcept
	{
		unsigned int offset = (n_bits / 8) % 64;
		unsigned int padding_length = offset < 56 ? 56 - offset : (56 + 64) - offset;
		append(PADDING, padding_length);
		n_bits -= (uint64_t)padding_length * 8;

		uint32_t block[16];
		for (unsigned int j = 0; j < 14; ++j)
		{
			block[j] = (uint32_t)(buffer[(j * 4) + 3]) << 24 |
				(uint32_t)(buffer[(j * 4) + 2]) << 16 |
				(uint32_t)(buffer[(j * 4) + 1]) << 8 |
				(uint32_t)(buffer[(j * 4)]);
		}
		block[14] = (uint32_t)n_bits;
		block[15] = (uint32_t)(n_bits >> 32);
		md5Step(state, block);
	}

	void md5::State::getDigest(uint8_t out[DIGEST_BYTES]) const noexcept
	{
		for (unsigned int i = 0; i < 4; ++i)
		{
			out[(i * 4) + 0] = (uint8_t)((state[i] & 0x000000FF));
			out[(i * 4) + 1] = (uint8_t)((state[i] & 0x0000FF00) >> 8);
			out[(i * 4) + 2] = (uint8_t)((state[i] & 0x00FF0000) >> 16);
			out[(i * 4) + 3] = (uint8_t)((state[i] & 0xFF000000) >> 24);
		}
	}

	std::string md5::State::getDigest() const SOUP_EXCAL
	{
		std::string digest(DIGEST_BYTES, '\0');
		getDigest((uint8_t*)digest.data());
		return digest;
	}
}
