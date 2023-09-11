#pragma once

#include "Capture.hpp"
#include "SelfDeletingThread.hpp"
#include "SharedPtr.hpp"

namespace soup
{
	class PromiseBase
	{
	protected:
		SharedPtr<Capture> res;

	public:
		PromiseBase()
			: res(soup::make_shared<Capture>())
		{
		}

	protected:
		PromiseBase(Capture&& res)
			: PromiseBase()
		{
			fulfil(std::move(res));
		}

	public:
		[[nodiscard]] bool isPending() const noexcept
		{
			return res->empty();
		}

		[[nodiscard]] bool isFulfilled() const noexcept
		{
			return !isPending();
		}

		void awaitCompletion();

	protected:
		void fulfil(Capture&& res) noexcept
		{
			*this->res = std::move(res);
		}
	};

	template <typename T>
	class Promise : public PromiseBase
	{
	public:
		Promise() = default;

		// Creates a fulfilled promise
		Promise(T&& res)
			: PromiseBase(std::move(res))
		{
		}

		Promise(T(*f)(Capture&&), Capture&& cap = {})
			: Promise()
		{
			fulfilOffThread(f, std::move(cap));
		}

		void fulfil(T&& res)
		{
			PromiseBase::fulfil(std::move(res));
		}

		[[nodiscard]] T& getResult() const noexcept
		{
			return res->get<T>();
		}

		void fulfilOffThread(T(*f)(Capture&&), Capture&& cap = {})
		{
			new SelfDeletingThread([](Capture&& _cap)
			{
				auto& cap = _cap.get<CaptureFulfillOffThread>();
				*cap.res = cap.f(std::move(cap.cap));
			}, CaptureFulfillOffThread{ res, f, std::move(cap) });
		}

	protected:
		struct CaptureFulfillOffThread
		{
			SharedPtr<Capture> res;
			T(*f)(Capture&&);
			Capture cap;
		};
	};
}
