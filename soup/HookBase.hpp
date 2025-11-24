#pragma once

#include "base.hpp"
#if SOUP_X86

#include <cstdint> // uint8_t

#include "Pattern.hpp"

NAMESPACE_SOUP
{
	struct HookBase
	{
		void* detour = nullptr;
		void* target = nullptr;

		static const uint8_t jmp_trampoline[5];
		static const uint8_t call_trampoline[5];
#if SOUP_BITS == 64
		static const uint8_t longjump_trampoline_r10[13];
		static const uint8_t longjump_trampoline_noreg[14];
#else
		static const uint8_t longjump_trampoline_noreg[6];
#endif

		[[nodiscard]] static Pattern getCodeCavePattern();

		[[nodiscard]] void* getEffectiveTarget() const;

		static void writeJmpTrampoline(void* addr, void* target);
		static void writeCallTrampoline(void* addr, void* target);
#if SOUP_BITS == 64
		static void writeLongjumpTrampolineR10(void* addr, void* target) noexcept;
#endif
		static void writeLongjumpTrampolineNoreg(void* addr, void* target) noexcept;
	};
}

#endif
