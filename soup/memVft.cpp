#include "memVft.hpp"

#include "Pointer.hpp"
#include "rtti.hpp"

namespace soup
{
	size_t memVft::getNumFunctions() const noexcept
	{
		size_t i = 0;
		//for (; getFunction(i) != nullptr; ++i);
		for (void** pVf = &vft[0]; *pVf != nullptr; ++pVf)
		{
			++i;
		}
		return i;
	}

	void* memVft::getFunction(size_t i) const noexcept
	{
		return vft[i];
	}

#if SOUP_WINDOWS
	bool memVft::hasRtti() const noexcept
	{
		return Pointer(vft[-1]).isInModule();
	}
#endif

	RttiObject* memVft::getRtti() const noexcept
	{
		return RttiObject::fromVft(vft);
	}
}
