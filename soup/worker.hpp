#pragma once

#include "fwd.hpp"

#include <cstdint>
#include <memory>
#include <utility>

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
		void(*holdup_callback)(worker&, capture&&);
		capture holdup_capture;
		void* holdup_data;

		virtual ~worker() = default;

		void operator=(worker&& b) noexcept;

		void fireHoldupCallback();
	protected:
		void awaitPromiseCompletion(promise_base* p, void(*f)(worker&, capture&&), capture&& cap);
	};
}
