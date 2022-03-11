#pragma once

#include "fwd.hpp"

#include <cstdint>
#include <memory>
#include <utility>

#include "callback.hpp"
#include "capture.hpp"

namespace soup
{
	class worker
	{
	public:
		enum holdup_type_t : uint8_t
		{
			NONE,
			SOCKET,
			PROMISE,
		};

		holdup_type_t holdup_type = NONE;
		callback<void(worker&)> holdup_callback;
		void* holdup_data;

		virtual ~worker() = default;

		void operator=(worker&& b) noexcept;

		void fireHoldupCallback();
	protected:
		void awaitPromiseCompletion(promise_base* p, void(*f)(worker&, capture&&), capture&& cap);
	};
}
