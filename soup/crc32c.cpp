#include "crc32c.hpp"
#if SOUP_X86 && defined(_MSC_VER) && defined(SOUP_USE_INTRIN)

NAMESPACE_SOUP
{
	namespace intrin
    {
	    extern uint32_t crc32c_intrin(const uint8_t* data, size_t size, uint32_t initial) noexcept;
    }

    uint32_t crc32c::hash(const uint8_t* data, size_t size, uint32_t initial) noexcept
    {
        return intrin::crc32c_intrin(data, size, initial);
    }
}

#endif
