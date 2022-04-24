#pragma once

namespace soup
{
	template <typename T>
	struct UniquePtr
	{
		T* data = nullptr;

		UniquePtr() noexcept = default;

		UniquePtr(T* ptr) noexcept
			: data(ptr)
		{
		}

		template <typename T2, std::enable_if_t<std::is_base_of_v<T, T2>, int> = 0>
		UniquePtr(UniquePtr<T2>&& b) noexcept
			: data(reinterpret_cast<T*>(b.data))
		{
			b.data = nullptr;
		}

		~UniquePtr()
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

		UniquePtr<T>& operator =(UniquePtr<T>&& b) noexcept
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
	[[nodiscard]] std::enable_if_t<!std::is_array_v<T>, UniquePtr<T>> make_unique(Args&&...args)
	{
		return UniquePtr<T>(new T(std::forward<Args>(args)...));
	}
}
