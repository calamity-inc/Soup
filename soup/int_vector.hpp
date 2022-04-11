#pragma once

#include <cstring> // memcpy

#include "optimised.hpp"

namespace soup
{
	template <typename T>
	struct int_vector
	{
		size_t num_elms = 0;
		size_t max_elms = 0;
		T* data;

		explicit constexpr int_vector() noexcept = default;

		explicit int_vector(const int_vector<T>& b) noexcept
			: num_elms(b.num_elms), max_elms(b.max_elms)
		{
			data = (T*)malloc(max_elms * sizeof(T));
			memcpy(data, b.data, num_elms * sizeof(T));
		}

		explicit int_vector(int_vector<T>&& b) noexcept
			: num_elms(b.num_elms), max_elms(b.max_elms), data(b.data)
		{
			b.num_elms = 0;
			b.max_elms = 0;
		}

		~int_vector() noexcept
		{
			free();
		}

		void operator=(const int_vector<T>& b) noexcept
		{
			free();

			num_elms = b.num_elms;
			max_elms = b.max_elms;

			data = (T*)malloc(max_elms * sizeof(T));
			memcpy(data, b.data, num_elms * sizeof(T));
		}

		void operator=(int_vector<T>&& b) noexcept
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

		[[nodiscard]] T& operator[](size_t idx) noexcept
		{
			return data[idx];
		}

		[[nodiscard]] const T& operator[](size_t idx) const noexcept
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

		void emplace_back(T val) noexcept
		{
			if (num_elms == max_elms)
			{
				const auto old_data = optimised::trinary<T*>(max_elms == 0, nullptr, data);
				max_elms += (0x1000 / sizeof(T));
				data = (T*)malloc(max_elms * sizeof(T));
				memcpy(data, old_data, num_elms * sizeof(T));
				if (old_data != nullptr)
				{
					::free(old_data);
				}
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

		void clear() noexcept
		{
			num_elms = 0;
			if (max_elms != 0)
			{
				max_elms = 0;
				::free(data);
			}
		}

	private:
		void free() noexcept
		{
			if (max_elms != 0)
			{
				::free(data);
			}
		}
	};
}
