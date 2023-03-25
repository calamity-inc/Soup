#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	struct Uuid
	{
		union
		{
			uint8_t bytes[16 / sizeof(uint8_t)];
			uint16_t words[16 / sizeof(uint16_t)];
			uint32_t ints[16 / sizeof(uint32_t)];
			uint64_t longs[16 / sizeof(uint64_t)];
		};

		[[nodiscard]] static Uuid v4();

		[[nodiscard]] uint8_t getVersion() const;
		[[nodiscard]] std::string toString() const;
	};
	static_assert(sizeof(Uuid) == 16);
}
