#pragma once

#include "base.hpp"

#if SOUP_WINDOWS
#include <Windows.h>
#else
#include <pthread.h>
#endif

#include <functional>
#include <vector>

#include "Capture.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	class Thread
	{
	public:
#if SOUP_WINDOWS
		HANDLE handle = INVALID_HANDLE_VALUE;
#else
		pthread_t handle{};
		bool have_handle = false;
		bool running = false;
#endif
		void(*f)(Capture&&);
		Capture f_cap;

		explicit Thread() noexcept = default;
		explicit Thread(void(*f)(Capture&&), Capture&& cap = {}) noexcept;
		explicit Thread(std::function<void()>&& func) noexcept;
		explicit Thread(const Thread& b) = delete;
		explicit Thread(Thread&& b) = delete;
		void start(void(*f)(Capture&&), Capture&& cap);
		void start(std::function<void()>&& func);

		~Thread() noexcept;

		void setTimeCritical() noexcept;

		[[nodiscard]] bool isRunning() const noexcept;
		void stop() noexcept;

		void awaitCompletion() noexcept;
		static void awaitCompletion(const std::vector<UniquePtr<Thread>>& threads) noexcept;
	};
}
