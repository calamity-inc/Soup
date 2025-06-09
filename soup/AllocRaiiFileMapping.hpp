#pragma once

#include "filesystem.hpp"

NAMESPACE_SOUP
{
	template <typename T = void>
	struct AllocRaiiFileMapping
	{
		const T* data;
		size_t size;

		constexpr AllocRaiiFileMapping() noexcept
			: data(nullptr), size(0)
		{
		}

		AllocRaiiFileMapping(const void* data, size_t size) noexcept
			: data((const T*)data), size(size)
		{
		}

		void init(const void* data, size_t size) noexcept
		{
			this->data = (const T*)data;
			this->size = size;
		}

		~AllocRaiiFileMapping() noexcept
		{
			filesystem::destroyFileMapping(data, size);
		}

		void reset() noexcept
		{
			if (data)
			{
				filesystem::destroyFileMapping(data, size);
				data = nullptr;
			}
		}
	};
}
