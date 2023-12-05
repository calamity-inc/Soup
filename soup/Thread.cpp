#include "Thread.hpp"

#include "Exception.hpp"
#include "format.hpp"
#include "SelfDeletingThread.hpp"

namespace soup
{
	Thread::Thread(void(*f)(Capture&&), Capture&& cap)
	{
		start(f, std::move(cap));
	}

	static void
#if SOUP_WINDOWS
		__stdcall
#endif
		threadCreateCallback(void* handover)
	{
		auto t = reinterpret_cast<Thread*>(handover);
		t->f(std::move(t->f_cap));
		t->running = false;
		t->f_cap.reset();
		if (t->is_self_deleting)
		{
			delete static_cast<SelfDeletingThread*>(t);
		}
	}

	void Thread::start(void(*f)(Capture&&), Capture&& cap)
	{
		SOUP_ASSERT(!isRunning());

		this->f = f;
		this->f_cap = std::move(cap);

#if SOUP_WINDOWS
		// if we still have a handle, relinquish it
		if (handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
		}

		handle = CreateThread(nullptr, 0, reinterpret_cast<DWORD(__stdcall*)(LPVOID)>(&threadCreateCallback), this, 0, nullptr);
		SOUP_IF_UNLIKELY (handle == NULL)
		{
			handle = INVALID_HANDLE_VALUE;
			SOUP_THROW(Exception(format("Failed to create thread: {}", GetLastError())));
		}
#else
		// if we still have a handle, relinquish it
		awaitCompletion();

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		auto ret = pthread_create(&handle, &attr, reinterpret_cast<void*(*)(void*)>(&threadCreateCallback), this);
		SOUP_IF_UNLIKELY (ret != 0)
		{
			SOUP_THROW(Exception(format("Failed to create thread: {}", ret)));
		}
		have_handle = true;
#endif

		running = true;
	}

	Thread::~Thread() noexcept
	{
#if SOUP_WINDOWS
		if (handle != INVALID_HANDLE_VALUE)
		{
			if (running)
			{
				TerminateThread(handle, 0);
			}
			CloseHandle(handle);
		}
#else
		if (have_handle)
		{
			pthread_detach(handle);
			pthread_cancel(handle);
		}
#endif
	}

#if SOUP_WINDOWS || (SOUP_POSIX && !SOUP_MACOS)
	void Thread::setTimeCritical() noexcept
	{
#if SOUP_WINDOWS
		SetThreadPriority(handle, THREAD_PRIORITY_TIME_CRITICAL);
#else
		pthread_setschedprio(handle, 15);
#endif
	}
#endif

	void Thread::stop() noexcept
	{
#if SOUP_WINDOWS
		TerminateThread(handle, 0);
		running = false;
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
