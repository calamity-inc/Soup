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
	private:
		[[nodiscard]] Pointer scanSimd(const Pattern& sig) const noexcept;
	public:
		[[nodiscard]] std::vector<Pointer> scanAll(const Pattern& sig, unsigned int limit = -1) const;
	private:
		[[nodiscard]] std::vector<Pointer> scanAllSimd(const Pattern& sig, unsigned int limit) const noexcept;
	};
}
