#pragma once

#include <cstring> // memcpy

#include "branchless.hpp"

namespace soup
{
	template <typename T>
	class IntVector
	{
	public:
		size_t num_elms = 0;
		size_t max_elms = 0;
		T* data;

		explicit constexpr IntVector() noexcept = default;

		explicit IntVector(const IntVector<T>& b) noexcept
			: num_elms(b.num_elms), max_elms(b.max_elms)
		{
			if (max_elms != 0)
			{
				data = (T*)malloc(max_elms * sizeof(T));
				memcpy(data, b.data, num_elms * sizeof(T));
			}
		}

		explicit IntVector(IntVector<T>&& b) noexcept
			: num_elms(b.num_elms), max_elms(b.max_elms), data(b.data)
		{
			b.num_elms = 0;
			b.max_elms = 0;
		}

		~IntVector() noexcept
		{
			free();
		}

		void operator=(const IntVector<T>& b) noexcept
		{
			free();

			num_elms = b.num_elms;
			max_elms = b.max_elms;

			if (max_elms != 0)
			{
				data = (T*)malloc(max_elms * sizeof(T));
				memcpy(data, b.data, num_elms * sizeof(T));
			}
		}

		void operator=(IntVector<T>&& b) noexcept
		{
			free();

			num_elms = b.num_elms;
			max_elms = b.max_elms;
			data = b.data;

			b.num_elms = 0;
			b.max_elms = 0;
		}

		[[nodiscard]] constexpr size_t size() const noexcept
		{
			return num_elms;
		}

		[[nodiscard]] SOUP_FORCEINLINE T& operator[](size_t idx) noexcept
		{
			return data[idx];
		}

		[[nodiscard]] SOUP_FORCEINLINE const T& operator[](size_t idx) const noexcept
		{
			return data[idx];
		}

		[[nodiscard]] T& at(size_t idx) noexcept
		{
			return data[idx];
		}

		[[nodiscard]] const T& at(size_t idx) const noexcept
		{
			return data[idx];
		}

		void emplaceZeroesFront(size_t elms) noexcept
		{
			SOUP_IF_UNLIKELY (num_elms + elms > max_elms)
			{
				// not optimal for performance, we'd memcpy twice, luckily this path is pretty cold
				makeSpaceForMoreElements();
				if (num_elms + elms > max_elms)
				{
					// this is also a possibility if elms >= 0x1000, shit will hit the fan...
				}
			}
			memmove(&data[elms], &data[0], num_elms * sizeof(T));
			memset(&data[0], 0, elms * sizeof(T));
			num_elms += elms;
		}

		void emplace_back(T val) noexcept
		{
			if (num_elms == max_elms)
			{
				makeSpaceForMoreElements();
			}
			data[num_elms] = val;
			++num_elms;
		}

		void erase(size_t idx) noexcept
		{
			if (--num_elms != idx)
			{
				memcpy(&data[idx], &data[idx + 1], (num_elms - idx) * sizeof(T));
			}
		}

		void eraseFirst(size_t num) noexcept
		{
			num_elms -= num;
			memcpy(&data[0], &data[num], num_elms * sizeof(T));
		}

		void preallocate() noexcept
		{
			if (max_elms == 0)
			{
				max_elms = (0x1000 / sizeof(T));
				data = (T*)malloc(max_elms * sizeof(T));
			}
		}

	protected:
		void makeSpaceForMoreElements() noexcept
		{
			const auto old_data = branchless::trinary<T*>(max_elms == 0, nullptr, data);
			max_elms += (0x1000 / sizeof(T));
			data = (T*)malloc(max_elms * sizeof(T));
			if (old_data != nullptr)
			{
				memcpy(data, old_data, num_elms * sizeof(T));
				::free(old_data);
			}
		}

	public:
		void clear() noexcept
		{
			num_elms = 0;
		}

		void reset() noexcept
		{
			clear();
			if (max_elms != 0)
			{
				max_elms = 0;
				::free(data);
			}
		}

	protected:
		void free() noexcept
		{
			if (max_elms != 0)
			{
				::free(data);
			}
		}
	};
}
