#include "thread.hpp"

namespace soup
{
	struct capture_thread_ctor_capture
	{
		void(*f)(capture&&);
		capture cap;
	};

	thread::thread(void(*f)(capture&&), capture&& cap) noexcept
	{
		create([](capture&& _cap)
		{
			auto& cap = _cap.get<capture_thread_ctor_capture>();
			cap.f(std::move(cap.cap));
		}, capture_thread_ctor_capture{
			f,
			std::move(cap)
		});
	}

	thread::thread(std::function<void()>&& func) noexcept
		: handle()
	{
		create([](capture&& cap)
		{
			cap.get<std::function<void()>>()();
		}, std::move(func));
	}

	struct capture_thread_create
	{
		void(*f)(capture&&);
		capture cap;
	};

	void thread::create(void(*f)(capture&&), capture&& cap) noexcept
	{
		create_capture = capture_thread_create{
			f,
			std::move(cap)
		};
		osCreate([](void* handover)
		{
			auto t = reinterpret_cast<thread*>(handover);
			auto& cap = t->create_capture.get<capture_thread_create>();
			cap.f(std::move(cap.cap));
#if !SOUP_WINDOWS
			t->running = false;
#endif
			t->create_capture.reset();
		}, this);
	}

	void thread::osCreate(void(*f)(void*), void* a) noexcept
	{
#if SOUP_WINDOWS
		handle = CreateThread(nullptr, 0, reinterpret_cast<DWORD(*)(void*)>(f), a, 0, nullptr);
#else
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&handle, &attr, reinterpret_cast<void*(*)(void*)>(f), a);
#endif
	}

	thread::~thread() noexcept
	{
#if SOUP_WINDOWS
		TerminateThread(handle, 0);
		CloseHandle(handle);
#else
		if (running)
		{
			pthread_cancel(handle);
			pthread_join(handle, nullptr);
		}
#endif
	}

	bool thread::isRunning() const noexcept
	{
#if SOUP_WINDOWS
		DWORD exit_code;
		return GetExitCodeThread(handle, &exit_code)
			&& exit_code == STILL_ACTIVE
			;
#else
		return running;
#endif
	}

	void thread::stop() noexcept
	{
#if SOUP_WINDOWS
		TerminateThread(handle, 0);
#else
		if (running)
		{
			pthread_cancel(handle);
			pthread_join(handle, nullptr);
			running = false;
		}
#endif
	}

	void thread::awaitCompletion() const noexcept
	{
#if SOUP_WINDOWS
		WaitForSingleObject(handle, INFINITE);
#else
		if (running)
		{
			pthread_join(handle, nullptr);
		}
#endif
	}

	void thread::awaitCompletion(const std::vector<thread>& threads) noexcept
	{
#if SOUP_WINDOWS
		std::vector<HANDLE> handles{};
		for (const auto& t : threads)
		{
			handles.emplace_back(t.handle);
		}
		WaitForMultipleObjects((DWORD)handles.size(), handles.data(), TRUE, INFINITE);
#else
		for (const auto& t : threads)
		{
			t.awaitCompletion();
		}
#endif
	}
}
