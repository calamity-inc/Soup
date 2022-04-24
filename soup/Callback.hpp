#pragma once

#include <functional>

#include "Capture.hpp"

namespace soup
{
	template <typename Func>
	struct Callback;

	template <typename Ret, typename...Args>
	struct Callback<Ret(Args...)>
	{
		using FuncT = Ret(Args...);
		using FuncWithCaptureT = Ret(Args..., const Capture&);

		FuncWithCaptureT* fp;
		Capture cap;

		Callback() noexcept
			: fp(nullptr)
		{
		}

		Callback(FuncT* fp) noexcept
			: fp(reinterpret_cast<FuncWithCaptureT*>(fp))
		{
		}
		
		Callback(FuncWithCaptureT* fp) noexcept
			: fp(fp)
		{
		}

		Callback(FuncWithCaptureT* fp, Capture&& cap) noexcept
			: fp(fp), cap(std::move(cap))
		{
		}

		static void redirect_to_std_function(Args... args, const Capture& cap)
		{
			return cap.get<std::function<Ret(Args...)>>()(std::forward<Args>(args)...);
		}

		template <typename T, std::enable_if_t<std::is_same_v<std::function<Ret(Args...)>, T>, int> = 0>
		Callback(T&& func) noexcept
			: Callback(&redirect_to_std_function, std::move(func))
		{
		}

		void set(FuncWithCaptureT* fp, Capture&& cap = {}) noexcept
		{
			this->fp = fp;
			this->cap = std::move(cap);
		}

		void operator=(FuncWithCaptureT* fp) noexcept
		{
			this->fp = fp;
			this->cap.reset();
		}

		void operator=(Callback&& b) noexcept
		{
			fp = b.fp;
			cap = std::move(b.cap);
		}

		void operator=(std::function<Ret(Args...)>&& func) noexcept
		{
			fp = &redirect_to_std_function;
			cap = std::move(func);
		}

		Ret operator() (Args&&...args)
		{
			return fp(std::forward<Args>(args)..., cap);
		}

		[[nodiscard]] constexpr operator bool() const noexcept
		{
			return fp != nullptr;
		}

		void reset()
		{
			fp = nullptr;
			cap.reset();
		}
	};
}
