#pragma once

#include <type_traits>
#include <utility> // move

#include "deleter.hpp"

namespace soup
{
	class capture
	{
	private:
		void* data;
		deleter_t m_deleter;

	public:
		capture() noexcept
			: data(nullptr)
		{
		}

		capture(const capture&) = delete;

		capture(capture&& b) noexcept
			: data(b.data), m_deleter(b.m_deleter)
		{
			b.data = nullptr;
		}

		template <typename T>
		capture(const T& v)
			: data(new std::remove_reference_t<T>(v)), m_deleter(&deleter<std::remove_reference_t<T>>)
		{
		}

		template <typename T>
		capture(T&& v)
			: data(new std::remove_reference_t<T>(std::move(v))), m_deleter(&deleter<std::remove_reference_t<T>>)
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
				m_deleter(data);
				data = nullptr;
			}
		}

		void operator =(const capture&) = delete;

		void operator =(capture&& b) noexcept
		{
			reset();
			data = b.data;
			m_deleter = b.m_deleter;
			b.data = nullptr;
		}

		template <typename T>
		void operator =(const T& v)
		{
			reset();
			data = new std::remove_reference_t<T>(v);
			m_deleter = &deleter<std::remove_reference_t<T>>;
		}

		template <typename T>
		void operator =(T&& v)
		{
			reset();
			data = new std::remove_reference_t<T>(std::move(v));
			m_deleter = &deleter<std::remove_reference_t<T>>;
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
