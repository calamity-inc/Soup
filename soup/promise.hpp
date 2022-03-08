#pragma once

#include "thread.hpp"

namespace soup
{
	template <typename T>
	class promise
	{
	private:
		thread thrd;

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
		T res{};

		promise(T(*f)(capture&&), capture&& cap = {})
			: thrd(&thrdFunc, capture_ctor{
				this,
				f,
				std::move(cap)
			})
		{
		}

		void awaitCompletion()
		{
			thrd.awaitCompletion();
		}
	};
}
