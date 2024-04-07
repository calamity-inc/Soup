#pragma once

#include "time.hpp"

NAMESPACE_SOUP
{
	class Timer
	{
	private:
		std::time_t deadline = 0;

	public:
		void start(std::time_t ms) noexcept
		{
			deadline = time::millis() + ms;
		}

		[[nodiscard]] bool isActive() const noexcept
		{
			return deadline != 0;
		}

		void reset() noexcept
		{
			deadline = 0;
		}

		[[nodiscard]] bool hasEnded() const noexcept
		{
			return time::millis() >= deadline;
		}
	};
}
