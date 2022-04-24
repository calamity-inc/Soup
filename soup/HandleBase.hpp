#pragma once

#include <Windows.h>

namespace soup
{
	struct HandleBase
	{
		HANDLE h;

		HandleBase() noexcept
			: h(INVALID_HANDLE_VALUE)
		{
		}

		HandleBase(const HandleBase&) = delete;

		HandleBase(HANDLE h) noexcept
			: h(h)
		{
		}

		void operator=(const HandleBase&) = delete;

		void operator=(HandleBase&& b)
		{
			this->~HandleBase();
			h = b.h;
			b.h = INVALID_HANDLE_VALUE;
		}

		void operator=(HANDLE h) noexcept
		{
			this->~HandleBase();
			this->h = h;
		}

		void set(HANDLE h) noexcept
		{
			this->~HandleBase();
			this->h = h;
		}

		virtual ~HandleBase() noexcept
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
	};
}
