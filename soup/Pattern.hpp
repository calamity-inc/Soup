#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

#include "fwd.hpp"
#include "base.hpp"
#if SOUP_WINDOWS
#include <Windows.h>
#endif

namespace soup
{
	struct Pattern
	{
		std::vector<std::optional<uint8_t>> bytes;

		explicit Pattern() = default;

		Pattern(const CompiletimePatternWithOptBytesBase& sig);
		Pattern(std::string_view ida_sig);

		void addBytesFromIdaSig(std::string_view ida_sig);

#if SOUP_WINDOWS
		[[nodiscard]] VirtualRegion virtual_scan(BYTE* startAddress = nullptr);
		[[nodiscard]] std::vector<VirtualRegion> virtual_scan_all(unsigned int limit = -1);
#endif
	};
}
