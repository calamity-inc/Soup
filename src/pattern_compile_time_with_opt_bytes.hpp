#pragma once

#include "pattern_compile_time_with_opt_bytes_base.hpp"

#include <array>

namespace soup
{
	template <size_t len>
	struct pattern_compile_time_with_opt_bytes : public pattern_compile_time_with_opt_bytes_base
	{
		std::array<std::optional<uint8_t>, len> aob{};

		constexpr pattern_compile_time_with_opt_bytes(const char* sig)
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

		std::vector<std::optional<std::uint8_t>> getVec() const final
		{
			return std::vector<std::optional<std::uint8_t>>(aob.begin(), aob.end());
		}
	};
}
