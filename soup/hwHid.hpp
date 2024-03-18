#pragma once

#include "base.hpp"

#include <cstdint>
#include <string>
#include <vector>

#include "Buffer.hpp"
#include "HandleRaii.hpp"

namespace soup
{
	// A human interface device.
	class hwHid
	{
	public:
		uint16_t vendor_id;
		uint16_t product_id;
		uint16_t usage;
		uint16_t usage_page;

	private:
#if SOUP_WINDOWS
		uint16_t output_report_byte_length;
		uint16_t feature_report_byte_length;
		bool pending_read = false;
		DWORD bytes_read{};
		OVERLAPPED read_overlapped{};
#else
		std::string manufacturer_name;
		std::string product_name;
		std::string serial_number;
#endif
		HandleRaii handle;
		Buffer read_buffer;

	public:
		[[nodiscard]] static std::vector<hwHid> getAll();

#if SOUP_WINDOWS
		// Note: Getting this information may require communicating with the device, which can be bad as some mice will shortly void input for this.
		[[nodiscard]] std::string getManufacturerName() const;
		[[nodiscard]] std::string getProductName() const;
		[[nodiscard]] std::string getSerialNumber() const;
#else
		[[nodiscard]] const std::string& getManufacturerName() const
		{
			return manufacturer_name;
		}

		[[nodiscard]] const std::string& getProductName() const
		{
			return product_name;
		}

		[[nodiscard]] const std::string& getSerialNumber() const
		{
			return serial_number;
		}
#endif

		[[nodiscard]] bool isSamePhysicalDeviceAs(const hwHid& b) const;

		// Do we have permission to send to and receive from the device?
		// On Windows, always.
		// On Linux, only as root or with special configuration: https://help.wooting.io/en/article/wootility-configuring-device-access-for-wootility-under-linux-udev-rules-r6lb2o/
		[[nodiscard]] bool havePermission() const noexcept
		{
#if SOUP_WINDOWS
			return true;
#else
			return handle.isValid();
#endif
		}

		[[nodiscard]] bool hasReportId(uint8_t report_id) const noexcept;

		[[nodiscard]] bool hasReport() noexcept;
		[[nodiscard]] const Buffer& receiveReport() noexcept; // blocking if !hasReport()
		void receiveFeatureReport(Buffer& buf) const noexcept;

		bool sendReport(Buffer&& buf) const noexcept;
		bool sendReport(const void* data, size_t size) const noexcept;
		bool sendFeatureReport(Buffer&& buf) const noexcept;

	private:
#if SOUP_WINDOWS
		void kickOffRead() noexcept;
#endif
	};
}
