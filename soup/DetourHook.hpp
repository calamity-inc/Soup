#pragma once

#include <cstdint>

namespace soup
{
	struct DetourHook
	{
		void* detour = nullptr;
		void* target = nullptr;
		void* original = nullptr;

		[[nodiscard]] void* getEffectiveTarget() const;

		void create();
		void remove();

		void enable();
		void disable();

		static void writeLongjumpTrampoline(void* addr, void* target);
	};
}
