#pragma once

#include <cmath> // ceil
#include <cstring> // strlen
#include <string>

#include "base.hpp"

#include "alpha2decodetbl.hpp"
#include "Bigint.hpp"
#include "bitutil.hpp"
#if SOUP_CPP20
#include "StringLiteral.hpp"
#endif

namespace soup
{
#if SOUP_CPP20
	template <StringLiteral AlphaStr>
#endif
	struct CustomEncoding
	{
#if SOUP_CPP20
		static constexpr const char* ALPHA = AlphaStr.c_str();
		static inline const Bigint ALPHA_SIZE = AlphaStr.size();

		static inline const auto decodetbl = alpha2decodetbl(AlphaStr.c_str());
#else
		const char* const ALPHA;
		const Bigint ALPHA_SIZE;
		const std::array<uint8_t, 256> decodetbl;

		CustomEncoding(const char* ALPHA)
			: ALPHA(ALPHA), ALPHA_SIZE(strlen(ALPHA)), decodetbl(alpha2decodetbl(ALPHA))
		{
		}
#endif

		// Note that this may be higher than the actual encoded length.
		[[nodiscard]]
#if SOUP_CPP20
		static
#endif
		size_t getEncodedLength(size_t inlen)
		{
			size_t alpha_size;
			SOUP_ASSERT(ALPHA_SIZE.toPrimitive(alpha_size));
			uint8_t seqlen = bitutil::getBitsNeededToEncodeRange(alpha_size);
			size_t bytelen = ceil(8.0f / seqlen);
			inlen *= bytelen;
			return ceil(((float)inlen * seqlen) / 8);
		}

		[[nodiscard]]
#if SOUP_CPP20
		static
#endif
		std::string encode(const std::string& msg)
		{
			return encode(Bigint::fromBinary(msg));
		}

		[[nodiscard]]
#if SOUP_CPP20
		static
#endif
		std::string encodeWithPadding(const std::string& msg)
		{
			auto enc = encode(msg);
			auto len = getEncodedLength(msg.size());
			while (enc.size() < len)
			{
				enc.insert(0, 1, ALPHA[0]);
			}
			return enc;
		}

		[[nodiscard]]
#if SOUP_CPP20
		static
#endif
		std::string encode(Bigint msg)
		{
			std::string enc{};
			while (!msg.isZero())
			{
				auto [q, r] = msg.divideUnsigned(ALPHA_SIZE);
				enc.insert(0, 1, ALPHA[r.getChunk(0)]);
				msg = std::move(q);
			}
			return enc;
		}

		// Note that leading zero-bytes will be trimmed both when encoding without padding, and by Bigint::toBinary during decoding, so if there is a certain length expectation, zeroes should be added to the front of the decoded data if that expectation is not met.
		[[nodiscard]]
#if SOUP_CPP20
		static
#endif
		std::string decode(const std::string& enc)
		{
			Bigint dec{};
			for (const auto& c : enc)
			{
				dec *= ALPHA_SIZE;
				dec += (Bigint::chunk_t)decodetbl[c];
			}
			return dec.toBinary();
		}
	};
}
