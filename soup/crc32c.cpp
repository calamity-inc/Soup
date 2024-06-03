#include "crc32c.hpp"
#if SOUP_X86 && defined(SOUP_USE_INTRIN)

namespace soup_intrin
{
	extern uint32_t crc32c_intrin(const uint8_t* data, size_t size, uint32_t initial) noexcept;
}

NAMESPACE_SOUP
{
    uint32_t crc32c::hash(const uint8_t* data, size_t size, uint32_t initial) noexcept
    {
        return ::soup_intrin::crc32c_intrin(data, size, initial);
    }
}

#endif
