#pragma once

#include <cstddef>
#include <cstdint>

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

		#if defined(__x86_64__) || defined(__ppc64__)
			#define SOUP_BITS 64
		#else
			#define SOUP_BITS 32
		#endif

		#define SOUP_EXPORT __attribute__ ((visibility ("default")))
	#endif
#endif

#define SOUP_CEXPORT extern "C" SOUP_EXPORT

#ifdef _MSC_VER
	#define SOUP_FORCEINLINE __forceinline
	#define SOUP_NOINLINE __declspec(noinline)
#else
	#define SOUP_FORCEINLINE __attribute__((always_inline))
	#define SOUP_NOINLINE __attribute__((noinline))
#endif

#if !defined(_MSC_VER) && (__cplusplus < 202002L)
#define SOUP_CPP20 false
#define consteval constexpr
#else
#define SOUP_CPP20 true
#endif

// emscripten seems to be missing std::string::operator<=>
#define SOUP_SPACESHIP (SOUP_CPP20 && !SOUP_WASM)

// platform-specific types, this is the only thing soup puts into the global namespace

#if SOUP_BITS == 64
	using halfintmax_t = int32_t;
	using halfsize_t = uint32_t;
#elif SOUP_BITS == 32
	using halfintmax_t = int16_t;
	using halfsize_t = uint16_t;
#endif
