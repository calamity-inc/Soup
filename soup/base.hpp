#pragma once

#include <cstddef>
#include <cstdint>

// === Determine platform (windows, wasm, or linux) and bits (32 or 64)

#ifdef _WIN32
	#define SOUP_WINDOWS true
	#define SOUP_WASM false
	#define SOUP_LINUX false

	#ifdef _WIN64
		#define SOUP_BITS 64
	#else
		#define SOUP_BITS 32
	#endif

	#define SOUP_EXPORT __declspec(dllexport)

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
#else
	#define SOUP_WINDOWS false

	#ifdef __EMSCRIPTEN__
		#define SOUP_WASM true
		#define SOUP_LINUX false

		#define SOUP_BITS 32

		#include "emscripten.h"
		#define SOUP_EXPORT EMSCRIPTEN_KEEPALIVE
	#else
		#define SOUP_WASM false
		#define SOUP_LINUX true

		#if defined(__x86_64__) || defined(__ppc64__) || defined(_M_X64) || defined(__aarch64__)
			#define SOUP_BITS 64
		#else
			#define SOUP_BITS 32
		#endif

		#define SOUP_EXPORT __attribute__ ((visibility ("default")))
	#endif
#endif

// === Determine CPU architecture

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__i386) || defined(__i486__) || defined(__i486) || defined(i386) || defined(__ia64__) || defined(__x86_64__)
	#define SOUP_X86 true
#else
	#define SOUP_X86 false
#endif

// === Determine if Soup is not being compiled as a static library

#ifndef SOUP_STANDALONE
	#if defined(_WINDLL) || SOUP_WASM
		#define SOUP_STANDALONE true
	#else
		#define SOUP_STANDALONE false
	#endif
#endif

// === Determine if code inspector

#ifdef __INTELLISENSE__
	#define SOUP_CODE_INSPECTOR true
#else
	#define SOUP_CODE_INSPECTOR false
#endif

// === Platform abstraction macros

#define SOUP_CEXPORT extern "C" SOUP_EXPORT

#ifdef _MSC_VER
	#define SOUP_FORCEINLINE __forceinline
	#define SOUP_NOINLINE __declspec(noinline)
#else
	#define SOUP_FORCEINLINE __attribute__((always_inline))
	#define SOUP_NOINLINE __attribute__((noinline))
#endif

// === C++ version abstraction macros

#if __cplusplus == 1997'11L
	#error Please set the /Zc:__cplusplus compiler flag or manually adjust __cplusplus when using Soup.
#endif

#ifdef SOUP_CPP20
	#pragma message("Ignoring SOUP_CPP20 define, this is automatically set based on C++ version.")
	#undef SOUP_CPP20
#endif

#if __cplusplus < 2020'00L
	#define SOUP_CPP20 false
#else
	#define SOUP_CPP20 true
#endif

#if !SOUP_CPP20
	#define consteval constexpr
#endif

#if SOUP_CPP20
	#define SOUP_IF_LIKELY(cond) if (cond) [[likely]]
	#define SOUP_IF_UNLIKELY(cond) if (cond) [[unlikely]]
#else
	#define SOUP_IF_LIKELY(cond) if (cond)
	#define SOUP_IF_UNLIKELY(cond) if (cond)
#endif

#if __cplusplus < 2023'00L
	#define SOUP_CPP23 false
#else
	#define SOUP_CPP23 true
#endif

#if SOUP_CPP23
	#define SOUP_ASSUME(x) [[assume(x)]];
#else
	#define SOUP_ASSUME(x) ;
#endif

// === C++ feature abstraction macros

#if (__cpp_exceptions < 1997'11L) && (!defined(_MSC_VER) || defined(__clang__))
	#define SOUP_EXCEPTIONS false
	#define SOUP_THROW(x) ::soup::throwImpl(x);

	#include <stdexcept>

	namespace soup
	{
		[[noreturn]] void throwImpl(std::exception&& e);
	}
#else
	#define SOUP_EXCEPTIONS true
	#define SOUP_THROW(x) throw x;
#endif

// === Platform-specific types

namespace soup
{
#if SOUP_BITS == 64
	using halfintmax_t = int32_t;
	using halfsize_t = uint32_t;
#elif SOUP_BITS == 32
	using halfintmax_t = int16_t;
	using halfsize_t = uint16_t;
#endif
}

// === Development helper macros

namespace soup
{
	[[noreturn]] void throwAssertionFailed();
	[[noreturn]] void throwAssertionFailed(const char* what);
}
#define SOUP_ASSERT(x, ...) SOUP_IF_UNLIKELY (!(x)) { ::soup::throwAssertionFailed(__VA_ARGS__); }
#define SOUP_ASSERT_UNREACHABLE ::soup::throwAssertionFailed();

#ifndef NDEBUG
	#define SOUP_ASSERT_ARG(x) SOUP_ASSERT(x)
#else
	#define SOUP_ASSERT_ARG(x) ;
#endif

template <typename T> SOUP_FORCEINLINE void SOUP_UNUSED(T&&) {}
