#pragma once

#include <utility> // move

#include "deleter_impl.hpp"
#include "type_traits.hpp"

namespace soup
{
	class Capture
	{
	protected:
		void* data = nullptr;
		deleter_t deleter = nullptr;

	public:
		Capture() noexcept = default;

		Capture(const Capture&) = delete;

		Capture(Capture&& b) noexcept
			: data(b.data), deleter(b.deleter)
		{
			b.forget();
		}

		template <typename T, SOUP_RESTRICT(!std::is_pointer_v<std::remove_reference_t<T>>)>
		Capture(const T& v)
			: data(new std::remove_reference_t<T>(v)), deleter(&deleter_impl<std::remove_reference_t<T>>)
		{
		}

		template <typename T, SOUP_RESTRICT(!std::is_pointer_v<std::remove_reference_t<T>>)>
		Capture(T&& v)
			: data(new std::remove_reference_t<T>(std::move(v))), deleter(&deleter_impl<std::remove_reference_t<T>>)
		{
		}

		template <typename T, SOUP_RESTRICT(std::is_pointer_v<std::remove_reference_t<T>>)>
		Capture(T v)
			: data(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(v)))
		{
		}

		~Capture()
		{
			free();
		}

		void reset() noexcept
		{
			free();
			forget();
		}

	protected:
		void free() noexcept
		{
			if (deleter != nullptr)
			{
				deleter(data);
			}
		}

		void forget() noexcept
		{
			data = nullptr;
			deleter = nullptr;
		}

	public:
		void operator =(const Capture&) = delete;

		void operator =(Capture&& b) noexcept
		{
			free();
			data = b.data;
			deleter = b.deleter;
			b.forget();
		}

		template <typename T, SOUP_RESTRICT(!std::is_pointer_v<std::remove_reference_t<T>>)>
		void operator =(const T& v)
		{
			free();
			data = new std::remove_reference_t<T>(v);
			deleter = &deleter_impl<std::remove_reference_t<T>>;
		}

		template <typename T, SOUP_RESTRICT(!std::is_pointer_v<std::remove_reference_t<T>>)>
		void operator =(T&& v)
		{
			free();
			data = new std::remove_reference_t<T>(std::move(v));
			deleter = &deleter_impl<std::remove_reference_t<T>>;
		}

		template <typename T, SOUP_RESTRICT(std::is_pointer_v<std::remove_reference_t<T>>)>
		void operator =(T v)
		{
			free();
			data = v;
			deleter = nullptr;
		}

		[[nodiscard]] operator bool() const noexcept
		{
			return data != nullptr;
		}

		template <typename T, SOUP_RESTRICT(!std::is_pointer_v<std::remove_reference_t<T>>)>
		[[nodiscard]] T& get() const noexcept
		{
			return *reinterpret_cast<T*>(data);
		}

		template <typename T, SOUP_RESTRICT(std::is_pointer_v<std::remove_reference_t<T>>)>
		[[nodiscard]] T get() const noexcept
		{
			return reinterpret_cast<T>(data);
		}
	};
}
