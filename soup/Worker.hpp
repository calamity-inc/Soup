#pragma once

#include "fwd.hpp"

#include <cstdint>
#include <utility>

#include "Callback.hpp"
#include "Capture.hpp"

namespace soup
{
	class Worker
	{
	public:
		enum holdup_type_t : uint8_t
		{
			NONE,
			SOCKET,
			PROMISE,
		};

		uint8_t recursions = 0;
		holdup_type_t holdup_type = NONE;
		Callback<void(Worker&)> holdup_callback;
		void* holdup_data;

		virtual ~Worker() = default;

		void operator=(Worker&& b) noexcept;

		void fireHoldupCallback();
	protected:
		void awaitPromiseCompletion(PromiseBase* p, void(*f)(Worker&, Capture&&), Capture&& cap);

	public:
		[[nodiscard]] bool canRecurse() noexcept;
	};
}
