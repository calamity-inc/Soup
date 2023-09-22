#include "DetourHook.hpp"

#include "Exception.hpp"
#include "memProtFlags.hpp"
#include "os.hpp"
#include "Pointer.hpp"
#include "x64.hpp"

namespace soup
{
	static uint8_t longjump_trampoline[] = {
		0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs r10, (8 bytes)
		0x41, 0xff, 0xe2, // jmp r10
	};

	//static uint8_t longjump_trampoline[] = {
	//	0x49, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs r11, (8 bytes)
	//	0x41, 0xff, 0xe3, // jmp r11
	//};

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
				throw Exception("Unsupported instruction");
			}
			og_bytes += (uint8_t)(op - op_start);
			for (const auto& opr : ins.operands)
			{
				if (opr.reg == soup::IP
					|| opr.reg == soup::DIS
					)
				{
					throw Exception("Instruction interacts with instruction pointer");
				}
			}
		} while (og_bytes < sizeof(longjump_trampoline));

		original = malloc(og_bytes + sizeof(longjump_trampoline));
		os::changeProtection(original, og_bytes + sizeof(longjump_trampoline), MEM_PROT_READ | MEM_PROT_WRITE | MEM_PROT_EXEC);
		memcpy(original, effective_target, og_bytes);
		writeLongjumpTrampoline((uint8_t*)original + og_bytes, (uint8_t*)effective_target + og_bytes);
	}

	void DetourHook::remove()
	{
		if (original != nullptr)
		{
			free(original);
			original = nullptr;
		}
	}

	void DetourHook::enable()
	{
		writeLongjumpTrampoline(getEffectiveTarget(), detour);
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
