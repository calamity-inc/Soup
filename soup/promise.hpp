#pragma once

#include "thread.hpp"

namespace soup
{
	template <typename T>
	class promise
	{
	private:
		std::unique_ptr<thread> thrd{};
	public:
		T res{};
		
	private:
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
		promise(T&& res)
			: res(std::move(res))
		{
		}

		promise(T(*f)(capture&&), capture&& cap = {})
			: thrd(std::make_unique<thread>(&thrdFunc, capture_ctor{
				this,
				f,
				std::move(cap)
			}))
		{
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
}
