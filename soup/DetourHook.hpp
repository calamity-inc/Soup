#pragma once

#include <cstdint>

#include "base.hpp"

NAMESPACE_SOUP
{
	struct DetourHook
	{
		void* detour = nullptr;
		void* target = nullptr;
		void* original = nullptr;

		[[nodiscard]] void* getEffectiveTarget() const;

		[[nodiscard]] bool isCreated() const noexcept { return original != nullptr; }
		void create();
		void destroy();

		void enable();
		void disable();

		static void writeLongjumpTrampoline(void* addr, void* target);
	};
}
