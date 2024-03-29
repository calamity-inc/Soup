#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

#include "fwd.hpp"
#include "base.hpp"

namespace soup
{
	struct Pattern
	{
		std::vector<std::optional<uint8_t>> bytes;

		explicit Pattern() = default;

		Pattern(const CompiletimePatternWithOptBytesBase& sig);
		Pattern(std::string_view ida_sig);

		void addBytesFromIdaSig(std::string_view ida_sig);
	};
}
