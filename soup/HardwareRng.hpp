#pragma once

#include <cstdint>

namespace soup
{
	// For the purposes of this class, a hardware RNG source is such that no bit is predictable to an attacker even with unlimited resources.
	// As such, it uses RDSEED and not RDRAND on x86 processors.
	struct HardwareRng
	{
		[[nodiscard]] static bool isAvailable() noexcept;

		[[nodiscard]] static uint16_t generate16() noexcept;
		[[nodiscard]] static uint32_t generate32() noexcept;
		[[nodiscard]] static uint64_t generate64() noexcept;
	};
}
