#pragma once

#include "RecursiveMutex.hpp"

NAMESPACE_SOUP
{
	class Mutex
	{
	private:
		RecursiveMutex mutex;
		bool locked = false;

	public:
		void lock()
		{
			mutex.lock();
			SOUP_IF_UNLIKELY (locked)
			{
				mutex.unlock();
				soup::throwAssertionFailed("Attempt to obtain recursive lock on non-recursive mutex");
			}
			locked = true;
		}

		[[nodiscard]] bool tryLock()
		{
			if (mutex.tryLock())
			{
				SOUP_IF_UNLIKELY (locked)
				{
					mutex.unlock();
					soup::throwAssertionFailed("Attempt to obtain recursive lock on non-recursive mutex");
				}
				locked = true;
				return true;
			}
			return false;
		}

		void unlock() noexcept
		{
			locked = false;
			mutex.unlock();
		}
	};
}
