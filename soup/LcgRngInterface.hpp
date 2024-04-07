#pragma once

#include "RngInterface.hpp"

#include "LcgRng.hpp"

NAMESPACE_SOUP
{
	struct LcgRngInterface : public RngInterface
	{
		LcgRng rng;

		LcgRngInterface() = default;

		LcgRngInterface(uint64_t seed)
			: rng(seed)
		{
		}

		[[nodiscard]] uint64_t getState() const noexcept
		{
			return rng.state;
		}

		[[nodiscard]] uint64_t generate() final
		{
			return rng.generate();
		}
	};
}
