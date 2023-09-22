#include "DetourHook.hpp"

#include "Exception.hpp"
#include "memProtFlags.hpp"
#include "os.hpp"
#include "Pointer.hpp"
#include "x64.hpp"

namespace soup
{
	static constexpr auto longjump_trampoline_size = 13;

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
		} while (og_bytes < longjump_trampoline_size);

		original = malloc(og_bytes + longjump_trampoline_size);
		os::changeProtection(original, og_bytes + longjump_trampoline_size, MEM_PROT_READ | MEM_PROT_WRITE | MEM_PROT_EXEC);
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
		memcpy(getEffectiveTarget(), original, longjump_trampoline_size);
	}

	void DetourHook::writeLongjumpTrampoline(void* addr, void* target)
	{
		x64Instruction ins;
		ins.setOperationFromOpcode(0xB8); // mov
		ins.operands[0].setReg(R10, ACCESS_64);
		ins.operands[1].setImm(reinterpret_cast<uint64_t>(target));
		
		auto bytecode = ins.toBytecode();

		ins.setOperationFromOpcode(0xFF, 5); // jmp
		// operands[0] is already set up

		bytecode += ins.toBytecode();

		memcpy(addr, bytecode.data(), bytecode.size());
	}
}
