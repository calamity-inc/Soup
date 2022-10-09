#pragma once

#include <atomic>

#include "base.hpp"
#include "Exception.hpp"

namespace soup
{
	class TaskGuard
	{
	private:
		std::atomic_bool state = false;

	public:
		[[nodiscard]] bool begin() noexcept
		{
			bool expected = false;
			return state.compare_exchange_strong(expected, true);
		}

		void end()
		{
			bool expected = true;
			SOUP_IF_UNLIKELY (!state.compare_exchange_strong(expected, false))
			{
				// If the guard is used correctly, we'd only ever end while the the state is true, and this will never be reached.
				Exception::raiseLogicError();
			}
		}
	};
}
