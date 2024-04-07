#include "memObject.hpp"

#include "memVft.hpp"
#include "Module.hpp"

NAMESPACE_SOUP
{
#if SOUP_WINDOWS
	bool memObject::hasVft() const noexcept
	{
		const Module mod{ nullptr };
		return Pointer(*reinterpret_cast<void* const*>(inst)).isInModule(mod) // vft must be within module
			&& Pointer(**reinterpret_cast<void** const*>(inst)).isInModule(mod) // as do virtual functions
			;
	}
#endif

	memVft memObject::getVft() const noexcept
	{
		return memVft{ *reinterpret_cast<void** const*>(inst) };
	}
}
