#pragma once

#include "base.hpp"

#include <cstdint>
#include <cstddef>

NAMESPACE_SOUP
{
	struct HidReportDescriptor
	{
		uint16_t usage_page = 0;
		uint16_t usage = 0;
		uint16_t input_report_byte_length = 0; // including report id
		uint16_t output_report_byte_length = 0; // including report id
		uint16_t feature_report_byte_length = 0; // including report id

		[[nodiscard]] static HidReportDescriptor parse(const void* _rawdesc, size_t size);
	};
}
