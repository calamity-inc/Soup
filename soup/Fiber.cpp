#include "Fiber.hpp"

#if !SOUP_WASM

#if SOUP_WINDOWS
#include <Windows.h>
#endif

namespace soup
{
#if SOUP_WINDOWS
	void __stdcall Fiber::entry(void* _f)
#else
	void Fiber::entry()
#endif
	{
#if SOUP_WINDOWS
		Fiber* f = reinterpret_cast<Fiber*>(_f);
#else
		Fiber* f = Fiber::current();
#endif
		f->func(std::move(f->cap));

		f->func = nullptr;
		do
		{
			f->yield();
		} while (true);
	}

	Fiber::Fiber(func_t func, Capture&& cap) noexcept
		: func(func), cap(std::move(cap))
#if SOUP_WINDOWS
		, inst(CreateFiber(0, &entry, this))
#endif
	{
#if SOUP_LINUX
		getcontext(&ctx);
		ctx.uc_link = &ret_ctx;
		ctx.uc_stack.ss_sp = stack;
		ctx.uc_stack.ss_size = sizeof(stack);
		makecontext(&ctx, &entry, 0);
#endif
	}

	Fiber::~Fiber() noexcept
	{
#if SOUP_WINDOWS
		DeleteFiber(inst);
#endif
	}

	Fiber* Fiber::current() noexcept
	{
#if SOUP_WINDOWS
		return static_cast<Fiber*>(GetFiberData());
#else
		return _current;
#endif
	}

	void Fiber::yield() noexcept
	{
#if SOUP_WINDOWS
		SwitchToFiber(return_to);
#else
		swapcontext(&ctx, &ret_ctx);
#endif
	}

	void Fiber::run() noexcept
	{
#if SOUP_WINDOWS
		if (!IsThreadAFiber())
		{
			ConvertThreadToFiber(nullptr);
		}
		return_to = GetCurrentFiber();
		SwitchToFiber(inst);
#else
		_current = this;
		swapcontext(&ret_ctx, &ctx);
#endif
	}

	bool Fiber::hasFinished() const noexcept
	{
		return func == nullptr;
	}
}

#endif
