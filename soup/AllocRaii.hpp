#pragma once

#include <cstdlib>

namespace soup
{
	struct AllocRaii
	{
		void* data;

		AllocRaii() noexcept
			: data(nullptr)
		{
		}

		AllocRaii(void* data) noexcept
			: data(data)
		{
		}

		AllocRaii(size_t size)
			: AllocRaii(malloc(size))
		{
		}

		~AllocRaii() noexcept
		{
			release();
		}

		void release() noexcept
		{
			if (data != nullptr)
			{
				free(data);
				data = nullptr;
			}
		}

		void operator=(AllocRaii&& b) noexcept
		{
			release();
			data = b.data;
			b.data = nullptr;
		}

		operator bool() const noexcept
		{
			return data != nullptr;
		}

		operator void* () const noexcept
		{
			return data;
		}
	};
}
