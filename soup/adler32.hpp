#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace soup
{
	struct adler32
	{
		static constexpr uint32_t INITIAL = 1;

		[[nodiscard]] static uint32_t hash(const std::string& data);
		[[nodiscard]] static uint32_t hash(const char* buf, size_t len);
		[[nodiscard]] static uint32_t hash(const uint8_t* buf, size_t len, uint32_t init = INITIAL);
	};
}
