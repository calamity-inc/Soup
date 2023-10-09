#include "Worker.hpp"

#include "Promise.hpp"
#include "Socket.hpp"

namespace soup
{
	void Worker::awaitPromiseCompletion(PromiseBase* p)
	{
		if (p->isPending())
		{
			holdup_type = PROMISE;
			holdup_callback.fp = [](Worker&, Capture&&) {};
			holdup_data = p;
		}
		else
		{
			holdup_type = NONE;
		}
	}

	void Worker::awaitPromiseCompletion(PromiseBase* p, void(*f)(Worker&, Capture&&), Capture&& cap)
	{
		if (!p->isPending() && canRecurse())
		{
			f(*this, std::move(cap));
		}
		else
		{
			holdup_type = PROMISE;
			holdup_callback.set(f, std::move(cap));
			holdup_data = p;
		}
	}

	struct CaptureAwaitPromiseCompletion
	{
		UniquePtr<PromiseBase> p;
		void(*f)(Worker&, PromiseBase&, Capture&&);
		Capture cap;
	};

	void Worker::awaitPromiseCompletion(UniquePtr<PromiseBase>&& p, void(*f)(Worker&, PromiseBase&, Capture&&), Capture&& cap)
	{
		CaptureAwaitPromiseCompletion cs{ std::move(p), f, std::move(cap) };
		PromiseBase* pp = cs.p.get();
		awaitPromiseCompletion(pp, [](Worker& w, Capture&& cap)
		{
			CaptureAwaitPromiseCompletion& cs = cap.get<CaptureAwaitPromiseCompletion>();
			cs.f(w, *cs.p, std::move(cs.cap));
		}, std::move(cs));
	}

	std::string Worker::toString() const
	{
#if !SOUP_WASM
		if (type == WORKER_TYPE_SOCKET)
		{
			return static_cast<const Socket*>(this)->toString();
		}
#endif
		return "[Worker]";
	}
}
