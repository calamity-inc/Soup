#pragma once

#include <string>

#include "base.hpp"

#include "alpha2decodetbl.hpp"
#include "Bigint.hpp"
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
		static inline const Bigint ALPHA_SIZE = (AlphaStr.size() - 1);

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
