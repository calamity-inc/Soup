#pragma once

#include <type_traits>
#include <utility> // move

#include "deleter_impl.hpp"

namespace soup
{
	class Capture
	{
	protected:
		void* data;
		deleter_t deleter;

	public:
		Capture() noexcept
			: data(nullptr)
		{
		}

		Capture(const Capture&) = delete;

		Capture(Capture&& b) noexcept
			: data(b.data), deleter(b.deleter)
		{
			b.data = nullptr;
		}

		template <typename T>
		Capture(const T& v)
			: data(new std::remove_reference_t<T>(v)), deleter(&deleter_impl<std::remove_reference_t<T>>)
		{
		}

		template <typename T>
		Capture(T&& v)
			: data(new std::remove_reference_t<T>(std::move(v))), deleter(&deleter_impl<std::remove_reference_t<T>>)
		{
		}

		~Capture()
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

		void operator =(const Capture&) = delete;

		void operator =(Capture&& b) noexcept
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
