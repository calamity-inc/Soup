#include "parallel.hpp"
#if !SOUP_WASM

#include <thread>
#include <vector>

#include "Thread.hpp"

NAMESPACE_SOUP
{
	struct ParallelRangeIteration
	{
		unsigned int size;
		void(*callback)(unsigned int, const Capture&);
		const Capture& cap;
		unsigned int num_threads;

		ParallelRangeIteration(unsigned int size, void(*callback)(unsigned int, const Capture&), const Capture& cap)
			: size(size), callback(callback), cap(cap)
		{
		}
	};

	struct ParallelRangeThreadInfo
	{
		unsigned int i;
		const ParallelRangeIteration* iteration;
	};

	void parallel::iterateRange(unsigned int size, void(*callback)(unsigned int, const Capture&), const Capture& cap)
	{
		ParallelRangeIteration iteration(size, callback, cap);

		iteration.num_threads = std::thread::hardware_concurrency();
		if (iteration.num_threads > iteration.size)
		{
			iteration.num_threads = iteration.size;
		}

		std::vector<UniquePtr<Thread>> threads{};
		for (unsigned int i = 0; i != iteration.num_threads; ++i)
		{
			threads.emplace_back(soup::make_unique<Thread>([](Capture&& cap)
			{
				auto& info = cap.get<ParallelRangeThreadInfo>();
				do
				{
					info.iteration->callback(info.i, info.iteration->cap);
					info.i += info.iteration->num_threads;
				} while (info.i < info.iteration->size);
			}, ParallelRangeThreadInfo{
				i,
				&iteration
			}));
		}
		Thread::awaitCompletion(threads);
	}
}

#endif
