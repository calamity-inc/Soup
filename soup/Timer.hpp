#pragma once

#include "time.hpp"

namespace soup
{
	class Timer
	{
	private:
		std::time_t started_at;

	public:
		inline Timer() noexcept
		{
			start();
		}

		inline void start() noexcept
		{
			started_at = time::nanos();
		}

		inline void stop() noexcept
		{
			started_at = (time::nanos() - started_at);
		}

		[[nodiscard]] inline std::time_t getNanos() noexcept
		{
			return started_at;
		}

		[[nodiscard]] inline double getMs() noexcept
		{
			return getNanos() / 1000000.0;
		}
	};
}
