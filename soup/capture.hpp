#pragma once

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
			: data(new T(v)), deleter(&deleter_impl<T>)
		{
		}

		template <typename T>
		capture(T&& v)
			: data(new T(std::move(v))), deleter(&deleter_impl<T>)
		{
		}

		template <typename T>
		capture(T* ptr)
			: data(new T*(ptr)), deleter(&deleter_impl<T*>)
		{
		}

		template <typename Ret, typename...Args>
		capture(Ret(*f)(Args...))
			: data(new void*(reinterpret_cast<void*>(f))), deleter(&deleter_impl<void*>)
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
			data = b.data;
			deleter = b.deleter;
			b.data = nullptr;
		}

		template <typename T>
		void operator =(const T& v)
		{
			reset();
			data = new T(v);
			deleter = &deleter_impl<T>;
		}

		template <typename T>
		void operator =(T&& v)
		{
			reset();
			data = new T(std::move(v));
			deleter = &deleter_impl<T>;
		}

		template <typename T>
		void operator =(T* ptr)
		{
			reset();
			data = new T*(ptr);
			deleter = &deleter_impl<T*>;
		}

		template <typename Ret, typename...Args>
		void operator =(Ret(*f)(Args...))
		{
			reset();
			data = new void*(reinterpret_cast<void*>(f));
			deleter = &deleter_impl<void*>;
		}

		template <typename T>
		[[nodiscard]] T& get() const noexcept
		{
			return *reinterpret_cast<T*>(data);
		}
	};
}
