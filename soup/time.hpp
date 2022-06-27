#pragma once

#include <chrono>

namespace soup
{
	struct time
	{
		[[nodiscard]] static std::time_t unixSeconds() noexcept
		{
			return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		}

		[[nodiscard]] static std::time_t millis() noexcept
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		}

		[[nodiscard]] static std::time_t nanos() noexcept
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		}
	};
}
