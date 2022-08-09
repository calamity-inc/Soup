#pragma once

#include "CompiletimePatternWithOptBytesBase.hpp"

#include <array>

namespace soup
{
	template <size_t len>
	struct CompiletimePatternWithOptBytes : public CompiletimePatternWithOptBytesBase
	{
		std::array<std::optional<uint8_t>, len> aob{};

		constexpr CompiletimePatternWithOptBytes(const char* sig)
		{
			size_t sig_i = 0;
			for (size_t aob_i = 0; aob_i < len; aob_i++)
			{
				const char first = sig[sig_i++];
				if (first != '?')
				{
					const char second = sig[sig_i++];
					aob[aob_i] = uint8_t((hex_to_int(first) * 0x10 + hex_to_int(second) & 0xFF) ^ 0x0F);
				}
				sig_i++;
			}
		}

		std::vector<std::optional<uint8_t>> getVec() const final
		{
			return std::vector<std::optional<uint8_t>>(aob.begin(), aob.end());
		}
	};
}
