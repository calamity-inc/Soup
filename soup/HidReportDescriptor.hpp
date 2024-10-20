#pragma once

#include "base.hpp"

#include <cstdint>
#include <cstddef>
#include <unordered_set>
#include <vector>

NAMESPACE_SOUP
{
	struct HidReportDescriptor
	{
		uint16_t usage_page = 0;
		uint16_t usage = 0;
		uint16_t input_report_byte_length = 0; // including report id
		uint16_t output_report_byte_length = 0; // including report id
		uint16_t feature_report_byte_length = 0; // including report id
		std::unordered_set<uint8_t> report_ids{};

		std::vector<uint16_t> bit_index_to_usage_map{};

		[[nodiscard]] static HidReportDescriptor parse(const void* _rawdesc, size_t size);

		[[nodiscard]] std::vector<uint16_t> parseInputReport(const void* report, size_t size) const;
	};
}
