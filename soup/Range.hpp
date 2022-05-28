#pragma once

#include <optional>
#include <vector>

#include "fwd.hpp"

#include "Pointer.hpp"

namespace soup
{
	class Range
	{
	public:
		Pointer base;
		size_t size;

		Range() noexcept = default;
		Range(Pointer base, size_t size) noexcept;

		[[nodiscard]] Pointer end() const noexcept;

		[[nodiscard]] bool contains(Pointer h) const noexcept;

		[[nodiscard]] static bool pattern_matches(uint8_t* target, const std::optional<uint8_t>* sig, size_t length) noexcept;

		[[nodiscard]] Pointer scan(const Pattern& sig) const noexcept;
		[[nodiscard]] std::vector<Pointer> scan_all(const Pattern& sig, unsigned int limit = -1) const;
	};
}
