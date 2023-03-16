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
		HANDLE handle;
#else
		pthread_t handle;
		bool running = true;
		bool joined = false;
#endif
		Capture create_capture;

		explicit Thread(void(*f)(Capture&&), Capture&& cap) noexcept;
		explicit Thread(std::function<void()>&& func) noexcept;
		explicit Thread(const Thread& b) = delete;
		explicit Thread(Thread&& b) = delete;
		void start(void(*f)(Capture&&), Capture&& cap) noexcept;
		void start(std::function<void()>&& func) noexcept;

		~Thread() noexcept;

		void setTimeCritical() noexcept;

		[[nodiscard]] bool isRunning() const noexcept;
		void stop() noexcept;

		void awaitCompletion() noexcept;
		static void awaitCompletion(std::vector<UniquePtr<Thread>>& threads) noexcept;
	};
}
