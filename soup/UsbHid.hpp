#pragma once

#include "base.hpp"

#include <string>
#include <vector>

#include "Buffer.hpp"

#if SOUP_WINDOWS
#include <Windows.h>

#include "HandleRaii.hpp"
#endif

namespace soup
{
	// A human interface device.
	struct UsbHid
	{
		uint16_t vendor_id;
		uint16_t product_id;
		uint16_t usage_page;

		uint16_t input_report_byte_length;
		uint16_t output_report_byte_length;
		uint16_t feature_report_byte_length;
#if SOUP_WINDOWS
		HandleRaii handle;
#endif

		[[nodiscard]] static std::vector<UsbHid> getAll();

		[[nodiscard]] std::string getProductName() const;
		[[nodiscard]] std::string getSerialNumber() const;

		[[nodiscard]] Buffer pollReport() const; // blocking

		void sendReport(Buffer&& buf) const;
		void sendFeatureReport(Buffer&& buf) const;
	};
}
