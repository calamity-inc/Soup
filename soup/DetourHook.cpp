#include "DetourHook.hpp"

#include <cstring> // memcpy

#include "Exception.hpp"
#include "memGuard.hpp"
#include "ObfusString.hpp"
#include "Pointer.hpp"
#include "x64.hpp"

#define DH_DEBUG false

#if DH_DEBUG
#include <iostream>
#include "string.hpp"
#endif

NAMESPACE_SOUP
{
	static uint8_t longjump_trampoline[] = {
		0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs r10, (8 bytes)
		0x41, 0xff, 0xe2, // jmp r10
	};

	void* DetourHook::getEffectiveTarget() const
	{
		return Pointer(target).followJumps().as<void*>();
	}

	void DetourHook::create()
	{
		const auto effective_target = getEffectiveTarget();
		auto op = (const uint8_t*)effective_target;
		uint8_t og_bytes = 0;
		do
		{
			auto op_start = op;
			auto ins = x64Disasm(op);
			if (!ins.isValid())
			{
				SOUP_THROW(Exception(ObfusString("Unsupported instruction").str()));
			}
			og_bytes += (uint8_t)(op - op_start);
			for (const auto& opr : ins.operands)
			{
				if (opr.reg == soup::IP
					|| opr.reg == soup::DIS
					)
				{
					SOUP_THROW(Exception(ObfusString("Instruction interacts with instruction pointer").str()));
				}
			}
		} while (og_bytes < sizeof(longjump_trampoline));

		original = malloc(og_bytes + sizeof(longjump_trampoline));
		memGuard::setAllowedAccess(original, og_bytes + sizeof(longjump_trampoline), memGuard::ACC_RWX);
		memcpy(original, effective_target, og_bytes);
		writeLongjumpTrampoline((uint8_t*)original + og_bytes, (uint8_t*)effective_target + og_bytes);

#if DH_DEBUG
		std::cout << "original proc: " << string::bin2hex(std::string((const char*)original, og_bytes + sizeof(longjump_trampoline))) << std::endl;
#endif
	}

	void DetourHook::destroy()
	{
		if (original != nullptr)
		{
			free(original);
			original = nullptr;
		}
	}

	void DetourHook::enable()
	{
		void* addr = getEffectiveTarget();
		memGuard::setAllowedAccess(addr, sizeof(longjump_trampoline), memGuard::ACC_RWX);
		writeLongjumpTrampoline(addr, detour);
	}

	void DetourHook::disable()
	{
		memcpy(getEffectiveTarget(), original, sizeof(longjump_trampoline));
	}

	void DetourHook::writeLongjumpTrampoline(void* addr, void* target)
	{
		*(void**)(longjump_trampoline + 2) = target;
		memcpy(addr, longjump_trampoline, sizeof(longjump_trampoline));
	}
}
