#pragma once

#include "base.hpp"

#include <string>
#include <vector>

#include "Buffer.hpp"
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
#if SOUP_WINDOWS
		uint16_t input_report_byte_length;
		uint16_t output_report_byte_length;
		uint16_t feature_report_byte_length;
#else
		std::string product_name;
		std::string serial_number;
#endif
		HandleRaii handle;

	public:
		[[nodiscard]] static std::vector<UsbHid> getAll();

#if SOUP_WINDOWS
		[[nodiscard]] std::string getProductName() const;
		[[nodiscard]] std::string getSerialNumber() const;
#else
		[[nodiscard]] const std::string& getProductName() const
		{
			return product_name;
		}

		[[nodiscard]] const std::string& getSerialNumber() const
		{
			return serial_number;
		}
#endif

		// Do we have permission to send to and receive from the device?
		// On Windows, always.
		// On Linux, only as root or with special configuration: https://help.wooting.io/en/article/wootility-configuring-device-access-for-wootility-under-linux-udev-rules-r6lb2o/
		[[nodiscard]] bool havePermission() const noexcept
		{
#if SOUP_WINDOWS
			return true;
#else
			return handle >= 0;
#endif
		}

		[[nodiscard]] Buffer pollReport() const; // blocking

		void sendReport(Buffer&& buf) const;
		void sendFeatureReport(Buffer&& buf) const;
	};
}
