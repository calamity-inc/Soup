#pragma once

#include <Windows.h>

namespace soup
{
	struct handle_base
	{
		HANDLE h;

		handle_base() noexcept
			: h(INVALID_HANDLE_VALUE)
		{
		}

		handle_base(HANDLE h) noexcept
			: h(h)
		{
		}

		virtual ~handle_base() noexcept
		{
		}

		operator HANDLE() const noexcept
		{
			return h;
		}

		template <typename T>
		operator T() const noexcept
		{
			return (T)h;
		}

		[[nodiscard]] operator bool() const noexcept
		{
			return valid();
		}

		[[nodiscard]] bool valid() const noexcept
		{
			return h != INVALID_HANDLE_VALUE;
		}

		void invalidate() noexcept
		{
			h = INVALID_HANDLE_VALUE;
		}

		void operator=(HANDLE h) noexcept
		{
			this->~handle_base();
			this->h = h;
		}
	};
}
