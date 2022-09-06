#include "CpuInfo.hpp"

#include "base.hpp"

#include "AllocRaiiLocalBase.hpp"
#include "os.hpp"
#include "string.hpp"
#include "UniquePtr.hpp"
#include "x64.hpp"

namespace soup
{
#define EAX arr[0]
#define EBX arr[1]
#define EDX arr[2]
#define ECX arr[3]

	CpuInfo::CpuInfo()
	{
		char buf[17];
		buf[16] = 0;
		invokeCpuid(buf, 0);
		cpuid_max_eax = *reinterpret_cast<uint32_t*>(&buf[0]);
		vendor_id = std::string(&buf[4]);

		if (cpuid_max_eax >= 0x01)
		{
			uint32_t arr[4];
			invokeCpuid(arr, 0x01);
			stepping_id = (EAX & 0xF);
			model = ((EAX >> 4) & 0xF);
			family = ((EAX >> 8) & 0xF);
			feature_flags_ecx = ECX;
			feature_flags_edx = EDX;

			if (cpuid_max_eax >= 0x16)
			{
				invokeCpuid(arr, 0x16);
				base_frequency = EAX;
				max_frequency = EBX;
				bus_frequency = ECX;
			}
		}
	}

	std::string CpuInfo::toString() const
	{
		std::string str = "CPUID Support Level: ";
		str.append(std::to_string(cpuid_max_eax));
		str.append("\nVendor: ");
		str.append(vendor_id);
		str.push_back('\n');

		if (cpuid_max_eax >= 0x01)
		{
			str.append("Stepping ID: ").append(std::to_string(stepping_id));
			str.append("\nModel: ").append(std::to_string(model));
			str.append("\nFamily: ").append(std::to_string(family));
			str.append("\nFeature Flags 1: ").append(string::hex(feature_flags_ecx));
			str.append("\nFeature Flags 2: ").append(string::hex(feature_flags_edx));
			str.push_back('\n');

			if (cpuid_max_eax >= 0x16)
			{
				str.append("Base Frequency: ").append(std::to_string(base_frequency)).append(
					" MHz\n"
					"Max. Frequency: "
				).append(std::to_string(max_frequency)).append(
					" MHz\n"
					"Bus (Reference) Frequency: "
				).append(std::to_string(bus_frequency)).append(" MHz\n");
			}
		}

		return str;
	}

	void CpuInfo::invokeCpuid(void* out, uint32_t eax)
	{
		static UniquePtr<AllocRaiiLocalBase> invoke_asm = os::allocateExecutable(x64Asm(
			"push esi\n"
#if SOUP_BITS == 64
			"mov rsi, rcx\n"
#else
			"mov esi, ecx\n"
#endif

			"mov eax, edx\n"
			//"xor ecx, ecx\n"
			"cpuid\n"

			"mov [esi], eax\n"
			"mov [esi+4], ebx\n"
			"mov [esi+8], edx\n"
			"mov [esi+12], ecx\n"

			"pop esi\n"
			"ret\n"
		));
		((void(*)(void*, uint32_t))invoke_asm->addr)(out, eax);
	}
}
