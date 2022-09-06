#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	struct CpuInfo
	{
		uint32_t cpuid_max_eax;
		std::string vendor_id;

		uint8_t stepping_id;
		uint8_t model;
		uint8_t family;
		uint32_t feature_flags_ecx;
		uint32_t feature_flags_edx;

		uint16_t base_frequency;
		uint16_t max_frequency;
		uint16_t bus_frequency;

		CpuInfo();

		[[nodiscard]] std::string toString() const;

		static void invokeCpuid(void* out, uint32_t eax);
	};
}
