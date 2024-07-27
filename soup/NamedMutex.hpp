#pragma once

#include "HandleRaii.hpp"

#if SOUP_WINDOWS

NAMESPACE_SOUP
{
	struct NamedMutex
	{
		HandleRaii h;

		NamedMutex(const char* name)
			: h(CreateMutexA(NULL, FALSE, name))
		{
			SOUP_ASSERT(h.isValid());
		}

		void lock()
		{
			WaitForSingleObject(h, INFINITE);
		}

		bool tryLock()
		{
			switch (WaitForSingleObject(h, 0))
			{
			case WAIT_OBJECT_0:
			case WAIT_ABANDONED:
				return true;
			}
			return false;
		}

		void unlock()
		{
			ReleaseMutex(h);
		}
	};
}

#endif
