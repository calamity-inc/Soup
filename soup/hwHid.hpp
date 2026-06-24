#pragma once

#include "base.hpp"

#include <cstdint>
#include <string>
#if !SOUP_WINDOWS
#include <unordered_set>
#include <pthread.h>
#endif
#include <vector>
#include <utility>

#include "Buffer.hpp"
#if SOUP_MACOS
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDManager.h>
#endif
#if !SOUP_MACOS
#include "HandleRaii.hpp"
#endif
#include "HidReportDescriptor.hpp"

NAMESPACE_SOUP
{
	// A human interface device.
	class hwHid
	{
	public:
		hwHid() = default;
		hwHid(const hwHid&) = delete;
		hwHid& operator=(const hwHid&) = delete;
		hwHid(hwHid&& other) noexcept
		{
			operator=(std::move(other));
		}

		hwHid& operator=(hwHid&& other) noexcept
		{
			if (this != &other)
			{
#if SOUP_MACOS
				if (device)
				{
					IOHIDDeviceClose((IOHIDDeviceRef)device, kIOHIDOptionsTypeNone);
					CFRelease((IOHIDDeviceRef)device);
				}
				device = other.device;
				other.device = nullptr;
				if (other.registered_callback)
				{
					uint8_t dummy;
					IOHIDDeviceRegisterInputReportCallback((IOHIDDeviceRef)device, &dummy, 0, nullptr, nullptr);
				}
#else
				handle = std::move(other.handle);
#endif
				path = std::move(other.path);
				vendor_id = other.vendor_id;
				product_id = other.product_id;
				usage_page = other.usage_page;
				usage = other.usage;
				input_report_byte_length = other.input_report_byte_length;
				output_report_byte_length = other.output_report_byte_length;
				feature_report_byte_length = other.feature_report_byte_length;
				is_bluetooth = other.is_bluetooth;
#if SOUP_WINDOWS
				pending_read = other.pending_read;
				disconnected = other.disconnected;
				bytes_read = other.bytes_read;
				read_overlapped = other.read_overlapped;
#else
				report_ids = std::move(other.report_ids);
				manufacturer_name = std::move(other.manufacturer_name);
				product_name = std::move(other.product_name);
				serial_number = std::move(other.serial_number);
	#if !SOUP_MACOS
				read_thrd = other.read_thrd;
				reading = other.reading;
	#endif
#endif
				read_buffer = std::move(other.read_buffer);
	#if SOUP_MACOS
				got_a_report = other.got_a_report;
	#endif
			}
			return *this;
		}

		std::string path;
		uint16_t vendor_id;
		uint16_t product_id;
		uint16_t usage_page;
		uint16_t usage;
		uint16_t input_report_byte_length; // including report id
		uint16_t output_report_byte_length; // including report id
		uint16_t feature_report_byte_length; // including report id
		bool is_bluetooth;

#if SOUP_WINDOWS
		uint8_t pending_read = 0;
		bool disconnected = false;
		DWORD bytes_read{};
		OVERLAPPED read_overlapped{};
#else
		std::unordered_set<uint8_t> report_ids{};
		std::string manufacturer_name;
		std::string product_name;
		std::string serial_number;
	#if !SOUP_MACOS
		pthread_t read_thrd;
		bool reading = false;
	#endif
#endif

	private:
#if SOUP_MACOS
		void* device = nullptr; // IOHIDDeviceRef
		bool registered_callback = false;
		bool got_a_report = false;
#else
		HandleRaii handle;
#endif
		Buffer<> read_buffer;

	public:
		[[nodiscard]] static std::vector<hwHid> getAll();

		[[nodiscard]] bool isValid() const noexcept { return !path.empty(); }

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
#elif SOUP_MACOS
			return device != nullptr;
#else
			return handle.isValid();
#endif
		}

		[[nodiscard]] bool isBluetooth() const noexcept { return is_bluetooth; }
		[[nodiscard]] bool hasReportId(uint8_t report_id) const noexcept;

		[[nodiscard]] bool hasReport() noexcept;
		[[nodiscard]] const Buffer<>& receiveReport() noexcept; // blocking if !hasReport()
		[[nodiscard]] const Buffer<>& receiveReport(uint8_t& out_report_id) noexcept; // blocking if !hasReport()
		[[nodiscard]] const Buffer<>& receiveReportWithReportId() noexcept; // blocking if !hasReport()
		[[nodiscard]] const Buffer<>& receiveReportWithoutReportId() noexcept; // blocking if !hasReport()
		void discardStaleReports() noexcept;
		void cancelReceiveReport() noexcept; // to be called from a different thread
		void receiveFeatureReport(Buffer<>& buf) const;

		bool sendReport(Buffer<>&& buf) const noexcept;
		bool sendReport(const void* data, size_t size) const noexcept;
		bool sendFeatureReport(Buffer<>&& buf) const noexcept;

		void reset() noexcept
		{
			path.clear();
#if SOUP_MACOS
			if (device)
			{
				// NOTE: deliberately NOT calling IOHIDDeviceUnscheduleFromRunLoop here. reset() can run on a
				// different thread than the one that scheduled the device (a static destructor during dylib
				// unload, or device-removal cleanup on the discovery thread). Unschedule uses
				// CFRunLoopGetCurrent(), which on the wrong/finalizing thread crashes on a freed run-loop mode
				// (observed as a bus error / NSException at uninitialise). reset() only runs when the device is
				// going away (disconnect or process exit), so its input callback won't fire again; just drop
				// our reference. Also do NOT IOHIDDeviceClose(): the persistent IOHIDManager owns each device's
				// open state, and getAll() hands out fresh hwHid wrappers around the SAME IOHIDDeviceRef on
				// every rediscovery, so closing a discarded duplicate would shut the device out from under an
				// active poll thread.
				registered_callback = false;
				CFRelease((IOHIDDeviceRef)device);
				device = nullptr;
			}
#else
			handle = HandleRaii();
#endif
		}

#if SOUP_MACOS
		~hwHid()
		{
			reset();
		}
#endif

	private:
#if SOUP_WINDOWS || SOUP_MACOS
		void kickOffRead() noexcept;
#endif

	public:
		[[nodiscard]] HidReportDescriptor getReportDescriptor() const;
	};
}
