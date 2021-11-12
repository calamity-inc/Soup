#pragma once

#include "pattern_compile_time_base.hpp"

#include <optional>
#include <vector>

namespace soup
{
	struct pattern_compile_time_with_opt_bytes_base : public pattern_compile_time_base
	{
		virtual std::vector<std::optional<uint8_t>> getVec() const = 0;
	};
}
