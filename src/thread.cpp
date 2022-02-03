#include "thread.hpp"

#if SOUP_PLATFORM_WINDOWS

namespace soup
{
	thread::thread(std::function<void()>&& func) noexcept
		: handle()
	{
		auto* fp = new std::function<void()>(std::move(func));
		this->handle.set(CreateThread(nullptr, 0, [](PVOID handover) -> DWORD
		{
			auto* fp = (std::function<void()>*)handover;
			(*fp)();
			delete fp;
			return 0;
		}, fp, 0, nullptr));
	}

	bool thread::isRunning() const noexcept
	{
		DWORD exit_code;
		return GetExitCodeThread(handle, &exit_code) && exit_code == STILL_ACTIVE;
	}
}

#endif
