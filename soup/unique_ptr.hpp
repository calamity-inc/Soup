#pragma once

namespace soup
{
	template <typename T>
	struct unique_ptr
	{
		T* data = nullptr;

		unique_ptr() noexcept = default;

		unique_ptr(T* ptr) noexcept
			: data(ptr)
		{
		}

		template <typename T2, std::enable_if_t<std::is_base_of_v<T, T2>, int> = 0>
		unique_ptr(unique_ptr<T2>&& b) noexcept
			: data(reinterpret_cast<T*>(b.data))
		{
			b.data = nullptr;
		}

		~unique_ptr()
		{
			if (data != nullptr)
			{
				delete data;
			}
		}

		void reset() noexcept
		{
			if (data != nullptr)
			{
				delete data;
				data = nullptr;
			}
		}

		unique_ptr<T>& operator =(unique_ptr<T>&& b) noexcept
		{
			reset();

			data = b.data;

			b.data = nullptr;

			return *this;
		}

		[[nodiscard]] operator bool() const noexcept
		{
			return data != nullptr;
		}

		[[nodiscard]] operator T*() const noexcept
		{
			return data;
		}

		[[nodiscard]] T* get() const noexcept
		{
			return data;
		}

		[[nodiscard]] T& operator*() const noexcept
		{
			return *data;
		}

		[[nodiscard]] T* operator->() const noexcept
		{
			return data;
		}
	};

	template <typename T, typename...Args>
	[[nodiscard]] std::enable_if_t<!std::is_array_v<T>, unique_ptr<T>> make_unique(Args&&...args)
	{
		return unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
}
