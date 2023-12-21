#pragma once

#include "base.hpp"
#if SOUP_WINDOWS || SOUP_LINUX
#include "Capture.hpp"

#if !SOUP_WINDOWS
#include <ucontext.h>
#endif

namespace soup
{
	class Fiber
	{
	public:
		using func_t = void(*)(Capture&&);

	private:
		func_t func;
		Capture cap;
#if SOUP_WINDOWS
		void* inst;
		void* return_to;
#else
		ucontext_t ctx;
		ucontext_t ret_ctx;
		char stack[1000000];

		inline static thread_local Fiber* _current;
#endif

#if SOUP_WINDOWS
		static void __stdcall entry(void*);
#else
		static void entry();
#endif

	public:
		explicit Fiber(func_t func, Capture&& cap = {}) noexcept;
		~Fiber() noexcept;

		[[nodiscard]] static Fiber* current() noexcept;
		void yield() noexcept;

		void run() noexcept;
		[[nodiscard]] bool hasFinished() const noexcept;
	};
}

#endif
