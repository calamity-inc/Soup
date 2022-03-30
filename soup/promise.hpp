#pragma once

#include "capture.hpp"
#include "thread.hpp"
#include "unique_ptr.hpp"

namespace soup
{
	class promise_base
	{
	protected:
		unique_ptr<thread> thrd{};
		capture res{};

		promise_base(unique_ptr<thread>&& thrd)
			: thrd(std::move(thrd))
		{
		}

		promise_base(capture&& res)
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
	class promise : public promise_base
	{
	protected:
		struct capture_ctor
		{
			promise* _this;
			T(*f)(capture&&);
			capture cap;
		};

		static void thrdFunc(capture&& _cap)
		{
			auto& cap = _cap.get<capture_ctor>();
			cap._this->res = cap.f(std::move(cap.cap));
		}

	public:
		promise(T(*f)(capture&&), capture&& cap = {})
			: promise_base(make_unique<thread>(&thrdFunc, capture_ctor{
				this,
				f,
				std::move(cap)
			}))
		{
		}

		promise(T&& res)
			: promise_base(std::move(res))
		{
		}

		promise(const promise& b) = delete;
		promise(promise&& b) = delete;

		[[nodiscard]] T& getResult() const noexcept
		{
			return res.get<T>();
		}
	};
}
