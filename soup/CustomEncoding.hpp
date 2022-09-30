#pragma once

#include <string>

#include "alpha2decodetbl.hpp"
#include "Bigint.hpp"
#include "StringLiteral.hpp"

namespace soup
{
	template <StringLiteral AlphaStr>
	struct CustomEncoding
	{
		static constexpr const char* ALPHA = AlphaStr.c_str();
		static inline const Bigint ALPHA_SIZE = (AlphaStr.size() - 1);

		static inline const auto decodetbl = alpha2decodetbl(AlphaStr.c_str());

		[[nodiscard]] static std::string encode(const std::string& msg)
		{
			return encode(Bigint::fromBinary(msg));
		}

		[[nodiscard]] static std::string encode(Bigint msg)
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

		[[nodiscard]] static std::string decode(const std::string& enc)
		{
			Bigint dec{};
			for (const auto& c : enc)
			{
				dec *= ALPHA_SIZE;
				dec += decodetbl[c];
			}
			return dec.toBinary();
		}
	};
}
