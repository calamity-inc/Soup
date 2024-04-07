#pragma once

#include "base.hpp"

#if SOUP_WINDOWS
#include "HandleBase.hpp"
#else
#include <unistd.h> // close
#endif

NAMESPACE_SOUP
{
#if SOUP_WINDOWS
	struct HandleRaii final : public HandleBase
	{
		using HandleBase::HandleBase;

		HandleRaii(HANDLE h) noexcept
			: HandleBase(h)
		{
		}

		HandleRaii(HandleRaii&& b) noexcept
			: HandleBase(b.h)
		{
			b.h = INVALID_HANDLE_VALUE;
		}

		~HandleRaii() noexcept final
		{
			if (isValid())
			{
				CloseHandle(h);
			}
		}

		using HandleBase::operator=;
	};
#else
	struct HandleRaii
	{
		int handle = -1;

		HandleRaii() noexcept = default;

		HandleRaii(int handle) noexcept
			: handle(handle)
		{
		}

		HandleRaii(HandleRaii&& b) noexcept
			: handle(b.handle)
		{
			b.handle = -1;
		}

		~HandleRaii() noexcept
		{
			if (isValid())
			{
				::close(handle);
			}
		}

		[[nodiscard]] bool isValid() const noexcept
		{
			return handle >= 0;
		}

		void operator=(int handle) noexcept
		{
			this->handle = handle;
		}

		operator int() const noexcept
		{
			return handle;
		}
	};
#endif
}
