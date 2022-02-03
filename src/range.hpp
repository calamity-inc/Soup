#pragma once

#include <optional>
#include <vector>

#include "fwddecl.hpp"

#include "pointer.hpp"

namespace soup
{
	class range
	{
	public:
		pointer base;
		size_t size;

		range() noexcept = default;
		range(pointer base, size_t size) noexcept;

		[[nodiscard]] pointer end() const noexcept;

		[[nodiscard]] bool contains(pointer h) const noexcept;

		[[nodiscard]] static bool pattern_matches(std::uint8_t* target, const std::optional<uint8_t>* sig, size_t length) noexcept;

		[[nodiscard]] pointer scan(const pattern& sig) const noexcept;
		[[nodiscard]] std::vector<pointer> scan_all(const pattern& sig, unsigned int limit = -1) const;
	};
}
