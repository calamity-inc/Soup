#pragma once

#include "base.hpp"

#if SOUP_WINDOWS
#include <Windows.h>
#else
#include <pthread.h>
#endif

#include <functional>
#include <vector>

#include "capture.hpp"

namespace soup
{
	class thread
	{
	public:
#if SOUP_WINDOWS
		HANDLE handle;
#else
		pthread_t handle;
		bool running = true;
		bool joined = false;
#endif
		capture create_capture;

		explicit thread(void(*f)(capture&&), capture&& cap) noexcept;
		explicit thread(std::function<void()>&& func) noexcept;
		explicit thread(const thread& b) = delete;
		explicit thread(thread&& b) = delete;
	private:
		void create(void(*f)(capture&&), capture&& cap) noexcept;

	public:
		~thread() noexcept;

		[[nodiscard]] bool isRunning() const noexcept;
		void stop() noexcept;

		void awaitCompletion() noexcept;
		static void awaitCompletion(std::vector<thread>& threads) noexcept;
	};
}
