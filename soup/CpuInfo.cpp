#include "CpuInfo.hpp"
#if SOUP_X86

#include "AllocRaiiVirtual.hpp"
#include "os.hpp"
#include "string.hpp"
#include "UniquePtr.hpp"
#include "x64.hpp"

#ifndef _MSC_VER
#include <cpuid.h>
#endif

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
		vendor_id = &buf[4];

		if (cpuid_max_eax >= 0x01)
		{
			uint32_t arr[4];
			invokeCpuid(arr, 0x01);
			stepping_id = (EAX & 0xF);
			model = ((EAX >> 4) & 0xF);
			family = ((EAX >> 8) & 0xF);
			feature_flags_ecx = ECX;
			feature_flags_edx = EDX;

			if (cpuid_max_eax >= 0x07)
			{
				invokeCpuid(arr, 0x07);
				extended_features_0_ebx = EBX;

				if (cpuid_max_eax >= 0x16)
				{
					invokeCpuid(arr, 0x16);
					base_frequency = EAX;
					max_frequency = EBX;
					bus_frequency = ECX;
				}
			}
		}
	}

	const CpuInfo& CpuInfo::get()
	{
		static CpuInfo inst;
		return inst;
	}

	std::string CpuInfo::toString() const
	{
		std::string str = "CPUID Support Level: ";
		str.append(std::to_string(cpuid_max_eax));
		str.append("\nVendor: ");
		str.append(vendor_id.c_str());
		str.push_back('\n');

		if (cpuid_max_eax >= 0x01)
		{
			str.append("Stepping ID: ").append(std::to_string(stepping_id));
			str.append("\nModel: ").append(std::to_string(model));
			str.append("\nFamily: ").append(std::to_string(family));
			str.append("\nFeature Flags 1: ").append(string::hex(feature_flags_ecx));
			str.append("\nFeature Flags 2: ").append(string::hex(feature_flags_edx));
			str.push_back('\n');

			if (cpuid_max_eax >= 0x07)
			{
				str.append("Feature Flags 3: ").append(string::hex(extended_features_0_ebx));
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
		}

		return str;
	}

	void CpuInfo::invokeCpuid(void* out, uint32_t eax)
	{
#ifdef _MSC_VER
		__cpuid(((int*)out), eax);
		std::swap(((int*)out)[2], ((int*)out)[3]);
#else
		__cpuid(eax, ((int*)out)[0], ((int*)out)[1], ((int*)out)[3], ((int*)out)[2]);
#endif
	}
}

#endif
