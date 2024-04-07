#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "fwd.hpp"
#include "base.hpp"

NAMESPACE_SOUP
{
	struct Pattern
	{
		std::vector<std::optional<uint8_t>> bytes;

		explicit Pattern() = default;
		Pattern(const CompiletimePatternWithOptBytesBase& sig);
		Pattern(const std::string& str);
		Pattern(const char* str, size_t len);

		[[nodiscard]] std::string toString() const SOUP_EXCAL;
	};
}
