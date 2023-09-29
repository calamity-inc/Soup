#pragma once

#include "base.hpp"
#if SOUP_WINDOWS

#include <Windows.h>

#include <string>
#include <vector>

#include "HandleRaii.hpp"

namespace soup
{
	// A human interface device.
	class UsbHid
	{
	public:
		uint16_t vendor_id;
		uint16_t product_id;
		uint16_t usage_page;
	private:
		uint16_t input_report_byte_length;
		HandleRaii handle;

	public:
		[[nodiscard]] static std::vector<UsbHid> getAll();

		[[nodiscard]] std::string pollReport() const;

		[[nodiscard]] uint16_t getReportLength() const noexcept
		{
			// Excluding report id added by Windows.
			return input_report_byte_length - 1;
		}
	};
}

#endif
