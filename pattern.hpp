#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

#include <Windows.h>

#include "fwddecl.hpp"

namespace soup
{
	struct pattern
	{
		std::vector<std::optional<uint8_t>> m_bytes;

		explicit pattern() = default;

		pattern(const pattern_compile_time_with_opt_bytes_base& sig);
		pattern(std::string_view ida_sig);

		void addBytesFromIdaSig(std::string_view ida_sig);

		[[nodiscard]] region_virtual virtual_scan(BYTE* startAddress = nullptr);
		[[nodiscard]] std::vector<region_virtual> virtual_scan_all(unsigned int limit = -1);
	};
}
