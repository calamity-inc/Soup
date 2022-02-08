#pragma once

#include <cstdlib>

namespace soup
{
	struct alloc_raii
	{
		void* data;

		alloc_raii() noexcept
			: data(nullptr)
		{
		}

		alloc_raii(void* data) noexcept
			: data(data)
		{
		}

		alloc_raii(size_t size)
			: alloc_raii(malloc(size))
		{
		}

		~alloc_raii() noexcept
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

		void operator=(alloc_raii&& b) noexcept
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
