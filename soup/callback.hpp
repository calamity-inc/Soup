#pragma once

#include "capture.hpp"

namespace soup
{
	template <typename Func>
	struct callback;

	template <typename Ret, typename...Args>
	struct callback<Ret(Args...)>
	{
		using FuncWithCapture = Ret(Args..., const capture&);

		FuncWithCapture* fp;
		capture cap;

		callback() noexcept
		{
		}

		callback(FuncWithCapture* fp) noexcept
			: fp(fp)
		{
		}

		callback(FuncWithCapture* fp, capture&& cap) noexcept
			: fp(fp), cap(std::move(cap))
		{
		}

		void set(FuncWithCapture* fp, capture&& cap = {}) noexcept
		{
			this->fp = fp;
			this->cap = std::move(cap);
		}

		void operator=(FuncWithCapture* fp) noexcept
		{
			this->fp = fp;
			this->cap.reset();
		}
		
		void operator=(callback&& b) noexcept
		{
			fp = b.fp;
			cap = std::move(b.cap);
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
