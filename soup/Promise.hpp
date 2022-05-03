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

		void fulfil(Capture&& res)
		{
			this->res = std::move(res);
		}
	};

	template <typename T>
	class Promise : public PromiseBase
	{
	protected:
		struct CaptureCtorRet
		{
			Promise* _this;
			T(*f)(Capture&&, PromiseBase*);
			Capture cap;
		};

		struct CaptureCtorVoid
		{
			Promise* _this;
			void(*f)(Capture&&, PromiseBase*);
			Capture cap;
		};

		static void thrdFuncRet(Capture&& _cap)
		{
			auto& cap = _cap.get<CaptureCtorRet>();
			cap._this->res = cap.f(std::move(cap.cap), cap._this);
		}

		static void thrdFuncVoid(Capture&& _cap)
		{
			auto& cap = _cap.get<CaptureCtorVoid>();
			cap.f(std::move(cap.cap), cap._this);
		}

	public:
		Promise(T(*f)(Capture&&, PromiseBase*), Capture&& cap = {})
			: PromiseBase(make_unique<Thread>(&thrdFuncRet, CaptureCtorRet{
				this,
				f,
				std::move(cap)
			}))
		{
		}

		Promise(void(*f)(Capture&&, PromiseBase*), Capture&& cap = {})
			: PromiseBase(make_unique<Thread>(&thrdFuncVoid, CaptureCtorVoid{
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
