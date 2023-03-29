#include "Benchmark.hpp"

#include <iostream>

namespace soup
{
	void Benchmark::run(const std::string& name, benchmark_t bm) noexcept
	{
		Benchmark::State state;
		bm(state);
		std::cout << name << ": " << (state.its / num_millis) << " iterations/ms\n";
	}
}
