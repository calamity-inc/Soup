#pragma once

#include <optional>
#include <vector>

#include "fwd.hpp"

#include "Pointer.hpp"

NAMESPACE_SOUP
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

		template <size_t S>
		[[nodiscard]] size_t scanWithMultipleResults(const Pattern& sig, Pointer(&buf)[S]) const noexcept
		{
			return scanWithMultipleResults(sig, buf, S);
		}

		[[nodiscard]] size_t scanWithMultipleResults(const Pattern& sig, Pointer buf[], size_t buflen) const noexcept;
	private:
		[[nodiscard]] size_t scanWithMultipleResultsSimd(const Pattern& sig, Pointer buf[], size_t buflen) const noexcept;
	};
}
