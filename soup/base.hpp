#pragma once

#include <cstdint>

#ifdef _WIN32
#define SOUP_WINDOWS true
#ifdef _WIN64
#define SOUP_BITS 64
#else
#define SOUP_BITS 32
#endif
#else
#define SOUP_WINDOWS false
#if defined(__x86_64__) || defined(__ppc64__)
#define SOUP_BITS 64
#else
#define SOUP_BITS 32
#endif
#endif

#ifdef _MSC_VER
#define SOUP_FORCEINLINE __forceinline
#define SOUP_NOINLINE __declspec(noinline)
#else
#define SOUP_FORCEINLINE __attribute__((always_inline))
#define SOUP_NOINLINE __attribute__((noinline))
#endif

// easy cross platform

#if !SOUP_WINDOWS
using size_t = std::size_t;
using ptrdiff_t = std::ptrdiff_t;
#endif

// platform-specific types, this is the only thing soup puts into the global namespace

#if SOUP_BITS == 64
using halfintmax_t = int32_t;
using halfsize_t = uint32_t;
#elif SOUP_BITS == 32
using halfintmax_t = int16_t;
using halfsize_t = uint16_t;
#endif
