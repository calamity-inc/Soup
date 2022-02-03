#pragma once

#include "handle_raii.hpp"

#include <functional>

namespace soup
{
	class thread
	{
	public:
		handle_raii handle;

		explicit thread(std::function<void()>&& func) noexcept;

		[[nodiscard]] bool isRunning() const noexcept;
	};
}
