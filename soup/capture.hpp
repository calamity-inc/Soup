#pragma once

#include <type_traits>

#include <utility> // move

namespace soup
{
	class capture
	{
	private:
		using deleter_t = void(*)(void*);

		void* data;
		deleter_t deleter;

		template <typename T>
		static void deleter_impl(void* ptr)
		{
			delete reinterpret_cast<T*>(ptr);
		}

	public:
		capture() noexcept
			: data(nullptr)
		{
		}

		capture(const capture&) = delete;

		capture(capture&& b) noexcept
			: data(b.data), deleter(b.deleter)
		{
			b.data = nullptr;
		}

		template <typename T>
		capture(const T& v)
			: data(new std::remove_reference_t<T>(v)), deleter(&deleter_impl<std::remove_reference_t<T>>)
		{
		}

		template <typename T>
		capture(T&& v)
			: data(new std::remove_reference_t<T>(std::move(v))), deleter(&deleter_impl<std::remove_reference_t<T>>)
		{
		}

		~capture()
		{
			reset();
		}

		void reset() noexcept
		{
			if (data != nullptr)
			{
				deleter(data);
				data = nullptr;
			}
		}

		void operator =(const capture&) = delete;

		void operator =(capture&& b) noexcept
		{
			reset();
			data = b.data;
			deleter = b.deleter;
			b.data = nullptr;
		}

		template <typename T>
		void operator =(const T& v)
		{
			reset();
			data = new std::remove_reference_t<T>(v);
			deleter = &deleter_impl<std::remove_reference_t<T>>;
		}

		template <typename T>
		void operator =(T&& v)
		{
			reset();
			data = new std::remove_reference_t<T>(std::move(v));
			deleter = &deleter_impl<std::remove_reference_t<T>>;
		}

		[[nodiscard]] operator bool() const noexcept
		{
			return data != nullptr;
		}

		template <typename T>
		[[nodiscard]] T& get() const noexcept
		{
			return *reinterpret_cast<T*>(data);
		}
	};
}
