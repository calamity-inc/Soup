#include "Thread.hpp"

#if SOUP_WINDOWS
#include "Exception.hpp"
#include "format.hpp"
#endif

namespace soup
{
	Thread::Thread(void(*f)(Capture&&), Capture&& cap) noexcept
	{
		start(f, std::move(cap));
	}

	Thread::Thread(std::function<void()>&& func) noexcept
	{
		start(std::move(func));
	}

	static void
#if SOUP_WINDOWS
		__stdcall
#endif
		threadCreateCallback(void* handover)
	{
		auto t = reinterpret_cast<Thread*>(handover);
		t->f(std::move(t->f_cap));
#if !SOUP_WINDOWS
		t->running = false;
#endif
		t->f_cap.reset();
	}

	void Thread::start(void(*f)(Capture&&), Capture&& cap)
	{
		this->f = f;
		this->f_cap = std::move(cap);
#if SOUP_WINDOWS
		// It's possible that Thread::stop was just called; this state is not immediately reflected.
		//SOUP_ASSERT(!isRunning());

		// if we still have a handle, relinquish it
		if (handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
		}

		handle = CreateThread(nullptr, 0, reinterpret_cast<DWORD(__stdcall*)(LPVOID)>(&threadCreateCallback), this, 0, nullptr);
		SOUP_IF_UNLIKELY (handle == NULL)
		{
			SOUP_THROW(Exception(format("Failed to create thread: {}", GetLastError())));
		}
#else
		SOUP_ASSERT(!isRunning());

		// if we still have a handle, relinquish it
		awaitCompletion();

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		SOUP_ASSERT(pthread_create(&handle, &attr, reinterpret_cast<void* (*)(void*)>(&threadCreateCallback), this) == 0);
		have_handle = true;
		running = true;
#endif
	}

	void Thread::start(std::function<void()>&& func)
	{
		start([](Capture&& cap)
		{
			cap.get<std::function<void()>>()();
		}, std::move(func));
	}

	Thread::~Thread() noexcept
	{
#if SOUP_WINDOWS
		TerminateThread(handle, 0);
		CloseHandle(handle);
#else
		if (have_handle)
		{
			pthread_detach(handle);
			pthread_cancel(handle);
		}
#endif
	}

	void Thread::setTimeCritical() noexcept
	{
#if SOUP_WINDOWS
		SetThreadPriority(handle, THREAD_PRIORITY_TIME_CRITICAL);
#else
		pthread_setschedprio(handle, 15);
#endif
	}

	bool Thread::isRunning() const noexcept
	{
#if SOUP_WINDOWS
		DWORD exit_code = 0;
		return GetExitCodeThread(handle, &exit_code)
			&& exit_code == STILL_ACTIVE
			;
#else
		return running;
#endif
	}

	void Thread::stop() noexcept
	{
#if SOUP_WINDOWS
		TerminateThread(handle, 0);
#else
		if (have_handle)
		{
			pthread_detach(handle);
			pthread_cancel(handle);
			have_handle = false;
			running = false;
		}
#endif
	}

	void Thread::awaitCompletion() noexcept
	{
#if SOUP_WINDOWS
		WaitForSingleObject(handle, INFINITE);
#else
		if (have_handle)
		{
			pthread_join(handle, nullptr);
			have_handle = false;
		}
#endif
	}

	void Thread::awaitCompletion(const std::vector<UniquePtr<Thread>>& threads) noexcept
	{
#if SOUP_WINDOWS
		std::vector<HANDLE> handles{};
		for (auto& t : threads)
		{
			handles.emplace_back(t->handle);
		}
		WaitForMultipleObjects((DWORD)handles.size(), handles.data(), TRUE, INFINITE);
#else
		for (auto& t : threads)
		{
			t->awaitCompletion();
		}
#endif
	}
}
