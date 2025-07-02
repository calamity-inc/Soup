#pragma once

#include <cstdint>
#include <string>

#include "base.hpp"

NAMESPACE_SOUP
{
	struct Uuid
	{
		union
		{
			uint8_t bytes[16 / sizeof(uint8_t)];
			uint16_t be_words[16 / sizeof(uint16_t)];
			uint32_t be_ints[16 / sizeof(uint32_t)];
			uint64_t be_longs[16 / sizeof(uint64_t)];
		};

		[[nodiscard]] static Uuid v4();

		[[nodiscard]] uint8_t getVersion() const;
		[[nodiscard]] std::string toString() const;
	};
	static_assert(sizeof(Uuid) == 16);
}
