#pragma once

#include "base.hpp"
#include "time.hpp"

NAMESPACE_SOUP
{
	struct Benchmark
	{
		static constexpr auto num_millis = 100;

		struct State
		{
			size_t its = 0;
			time_t deadline = 0;

			[[nodiscard]] SOUP_FORCEINLINE bool canContinue() noexcept
			{
				SOUP_IF_UNLIKELY (its == 0)
				{
					its = 1;
					deadline = time::millis() + num_millis;
					return true;
				}
				++its;
				return time::millis() < deadline;
			}
		};

		using benchmark_t = void(*)(Benchmark::State&);

		static void run(const std::string& name, benchmark_t bm) noexcept;
	};
}

#define BENCHMARK(name, ...) ::soup::Benchmark::run(name, [](::soup::Benchmark::State& _benchmark_state) { __VA_ARGS__ });
#define BENCHMARK_LOOP(...) while (true) { SOUP_IF_UNLIKELY (!_benchmark_state.canContinue()) { break; } __VA_ARGS__ }
