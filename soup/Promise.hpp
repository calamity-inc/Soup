#pragma once

#include "Capture.hpp"
#include "Thread.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	class PromiseBase
	{
	protected:
		UniquePtr<Thread> thrd{};
		Capture res{};

		PromiseBase(UniquePtr<Thread>&& thrd)
			: thrd(std::move(thrd))
		{
		}

		PromiseBase(Capture&& res)
			: res(std::move(res))
		{
		}

	public:
		[[nodiscard]] bool isPending() const noexcept
		{
			return thrd && thrd->isRunning();
		}

		void awaitCompletion()
		{
			if (thrd)
			{
				thrd->awaitCompletion();
				thrd.reset();
			}
		}
	};

	template <typename T>
	class Promise : public PromiseBase
	{
	protected:
		struct CaptureCtor
		{
			Promise* _this;
			T(*f)(Capture&&);
			Capture cap;
		};

		static void thrdFunc(Capture&& _cap)
		{
			auto& cap = _cap.get<CaptureCtor>();
			cap._this->res = cap.f(std::move(cap.cap));
		}

	public:
		Promise(T(*f)(Capture&&), Capture&& cap = {})
			: PromiseBase(make_unique<Thread>(&thrdFunc, CaptureCtor{
				this,
				f,
				std::move(cap)
			}))
		{
		}

		Promise(T&& res)
			: PromiseBase(std::move(res))
		{
		}

		Promise(const Promise& b) = delete;
		Promise(Promise&& b) = delete;

		[[nodiscard]] T& getResult() const noexcept
		{
			return res.get<T>();
		}
	};
}
