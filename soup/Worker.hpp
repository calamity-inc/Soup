#pragma once

#include "fwd.hpp"

#include <cstdint>
#include <utility>

#include "Callback.hpp"
#include "Capture.hpp"

namespace soup
{
	constexpr uint8_t WORKER_TYPE_UNSPECIFIED = 0;
	constexpr uint8_t WORKER_TYPE_SOCKET = 1;
	constexpr uint8_t WORKER_TYPE_USER = 2;

	struct Worker
	{
		enum HoldupType : uint8_t
		{
			NONE,
			SOCKET,
			IDLE, // call holdup callback whenever the scheduler has some idle time
			PROMISE,
			TASK,
		};

		uint8_t type;
		uint8_t recursions = 0;
		HoldupType holdup_type = NONE;
		Callback<void(Worker&)> holdup_callback;
		void* holdup_data;

		Worker(uint8_t type = WORKER_TYPE_UNSPECIFIED) noexcept
			: type(type)
		{
		}

		virtual ~Worker() = default;

		void operator=(Worker&& b) noexcept;

		void fireHoldupCallback();
		void awaitPromiseCompletion(PromiseBase* p, void(*f)(Worker&, Capture&&), Capture&& cap = {});
		void awaitPromiseCompletion(UniquePtr<PromiseBase>&& p, void(*f)(Worker&, PromiseBase&, Capture&&), Capture&& cap = {});
		void setWorkDone() noexcept;

		[[nodiscard]] bool canRecurse() noexcept;
	};
}
