#include "hwHid.hpp"

// Based on hidapi:
// - https://github.com/libusb/hidapi/blob/master/windows/hid.c
// - https://github.com/libusb/hidapi/blob/master/linux/hid.c

#if SOUP_WINDOWS
#include <cfgmgr32.h>
#include <hidsdi.h>

#pragma comment(lib, "CfgMgr32.lib")
#pragma comment(lib, "hid.lib")

#include "unicode.hpp"
#elif SOUP_LINUX
#include <sys/stat.h> // open
#include <fcntl.h> // O_RDWR etc
#include <unistd.h> // read
#include <poll.h>

#include <linux/hidraw.h>

#include "SharedLibrary.hpp"
#include "string.hpp"

// https://github.com/systemd/systemd/blob/main/src/libudev/libudev.h

struct udev;
struct udev_enumerate;
struct udev_list_entry;
struct udev_device;

using udev_new_t = udev*(*)();
using udev_unref_t = udev*(*)(udev*);

using udev_enumerate_unref_t = udev_enumerate*(*)(udev_enumerate*);
using udev_enumerate_new_t = udev_enumerate*(*)(udev*);
using udev_enumerate_add_match_subsystem_t = int(*)(udev_enumerate*, const char*);
using udev_enumerate_scan_devices_t = int(*)(udev_enumerate*);
using udev_enumerate_get_list_entry_t = udev_list_entry*(*)(udev_enumerate*);

using udev_list_entry_get_next_t = udev_list_entry*(*)(udev_list_entry*);
using udev_list_entry_get_name_t = const char*(*)(udev_list_entry*);

using udev_device_unref_t = udev_device*(*)(udev_device*);
using udev_device_new_from_syspath_t = udev_device*(*)(udev*, const char*);
using udev_device_get_devnode_t = const char*(*)(udev_device*);
using udev_device_get_parent_with_subsystem_devtype_t = udev_device*(*)(udev_device*, const char*, const char*);
using udev_device_get_sysattr_value_t = const char*(*)(udev_device*, const char*);

#define udev_list_entry_foreach(list_entry, first_entry) for (list_entry = first_entry; list_entry; list_entry = udev_list_entry_get_next(list_entry))
#endif

namespace soup
{
#if SOUP_LINUX
	[[nodiscard]] static bool parse_uevent_info(std::string uevent, unsigned short& vendor_id, unsigned short& product_id, std::string& product_name_utf8, std::string& serial_number_utf8)
	{
		char* saveptr = NULL;
		char* line;
		char* key;
		char* value;

		bool found_id = false;
		bool found_serial = false;
		bool found_name = false;

		line = strtok_r(uevent.data(), "\n", &saveptr);
		while (line != NULL)
		{
			/* line: "KEY=value" */
			key = line;
			value = strchr(line, '=');
			if (value)
			{
				*value = '\0';
				value++;

				if (strcmp(key, "HID_ID") == 0)
				{
					/**
					 *        type vendor   product
					 * HID_ID=0003:000005AC:00008242
					 **/
					unsigned bus_type;
					int ret = sscanf(value, "%x:%hx:%hx", &bus_type, &vendor_id, &product_id);
					if (ret == 3) {
						found_id = true;
					}
				}
				else if (strcmp(key, "HID_NAME") == 0)
				{
					product_name_utf8 = value;
					found_name = true;
				}
				else if (strcmp(key, "HID_UNIQ") == 0)
				{
					serial_number_utf8 = value;
					found_serial = true;
				}
			}
			line = strtok_r(NULL, "\n", &saveptr);
		}

		return (found_id && found_name && found_serial);
	}

	[[nodiscard]] static std::string get_uevent_from_sysfs(std::string path)
	{
		path.append("/device/uevent");
		return soup::string::fromFile(path);
	}

	/*
	 * Retrieves the hidraw report descriptor from a file.
	 * When using this form, <sysfs_path>/device/report_descriptor, elevated privileges are not required.
	 */
	static int get_hid_report_descriptor(const char* rpt_path, struct hidraw_report_descriptor* rpt_desc)
	{
		int rpt_handle;
		ssize_t res;

		rpt_handle = ::open(rpt_path, O_RDONLY | O_CLOEXEC);
		if (rpt_handle < 0) {
			printf("open failed (%s): %s", rpt_path, strerror(errno));
			return -1;
		}

		/*
		 * Read in the Report Descriptor
		 * The sysfs file has a maximum size of 4096 (which is the same as HID_MAX_DESCRIPTOR_SIZE) so we should always
		 * be ok when reading the descriptor.
		 * In practice if the HID descriptor is any larger I suspect many other things will break.
		 */
		memset(rpt_desc, 0x0, sizeof(*rpt_desc));
		res = read(rpt_handle, rpt_desc->value, HID_MAX_DESCRIPTOR_SIZE);
		if (res < 0) {
			printf("read failed (%s): %s", rpt_path, strerror(errno));
		}
		rpt_desc->size = (uint32_t)res;

		close(rpt_handle);
		return (int)res;
	}

	/* return size of the descriptor, or -1 on failure */
	static int get_hid_report_descriptor_from_sysfs(std::string path, struct hidraw_report_descriptor* rpt_desc)
	{
		path.append("/device/report_descriptor");
		return get_hid_report_descriptor(path.c_str(), rpt_desc);
	}

	/*
	 * Get bytes from a HID Report Descriptor.
	 * Only call with a num_bytes of 0, 1, 2, or 4.
	 */
	static uint32_t get_hid_report_bytes(const uint8_t* rpt, size_t len, size_t num_bytes, size_t cur)
	{
		/* Return if there aren't enough bytes. */
		if (cur + num_bytes >= len)
			return 0;

		if (num_bytes == 0)
			return 0;
		else if (num_bytes == 1)
			return rpt[cur + 1];
		else if (num_bytes == 2)
			return (rpt[cur + 2] * 256 + rpt[cur + 1]);
		else if (num_bytes == 4)
			return (
				rpt[cur + 4] * 0x01000000 +
				rpt[cur + 3] * 0x00010000 +
				rpt[cur + 2] * 0x00000100 +
				rpt[cur + 1] * 0x00000001
				);
		else
			return 0;
	}

	/*
	 * Gets the size of the HID item at the given position
	 * Returns 1 if successful, 0 if an invalid key
	 * Sets data_len and key_size when successful
	 */
	static bool get_hid_item_size(const uint8_t* report_descriptor, uint32_t size, unsigned int pos, int* data_len, int* key_size)
	{
		int key = report_descriptor[pos];
		int size_code;

		/*
		 * This is a Long Item. The next byte contains the
		 * length of the data section (value) for this key.
		 * See the HID specification, version 1.11, section
		 * 6.2.2.3, titled "Long Items."
		 */
		if ((key & 0xf0) == 0xf0) {
			if (pos + 1 < size)
			{
				*data_len = report_descriptor[pos + 1];
				*key_size = 3;
				return true;
			}
			*data_len = 0; /* malformed report */
			*key_size = 0;
		}

		/*
		 * This is a Short Item. The bottom two bits of the
		 * key contain the size code for the data section
		 * (value) for this key. Refer to the HID
		 * specification, version 1.11, section 6.2.2.2,
		 * titled "Short Items."
		 */
		size_code = key & 0x3;
		switch (size_code) {
		case 0:
		case 1:
		case 2:
			*data_len = size_code;
			*key_size = 1;
			return true;
		case 3:
			*data_len = 4;
			*key_size = 1;
			return true;
		default:
			/* Can't ever happen since size_code is & 0x3 */
			*data_len = 0;
			*key_size = 0;
			break;
		};

		/* malformed report */
		return false;
	}

	static void get_hid_usage(hwHid& hid, const uint8_t* report_descriptor, uint32_t size)
	{
		unsigned int pos = 0;
		while (pos < size)
		{
			int key = report_descriptor[pos];
			int key_cmd = key & 0xfc;

			int data_len, key_size;
			if (!get_hid_item_size(report_descriptor, size, pos, &data_len, &key_size))
			{
				break;
			}

			switch (key & 0xfc)
			{
			case 0x4: /* Usage Page 6.2.2.7 (Global) */
				hid.usage_page = get_hid_report_bytes(report_descriptor, size, data_len, pos);
				break;

			case 0x8: /* Usage 6.2.2.8 (Local) */
				if (data_len == 4) /* Usages 5.5 / Usage Page 6.2.2.7 */
				{
					hid.usage_page = get_hid_report_bytes(report_descriptor, size, 2, pos + 2);
					hid.usage = get_hid_report_bytes(report_descriptor, size, 2, pos);
				}
				else
				{
					hid.usage = get_hid_report_bytes(report_descriptor, size, data_len, pos);
				}
				break;
			}

			pos += data_len + key_size;
		}
	}
#endif

	std::vector<hwHid> hwHid::getAll()
	{
		std::vector<hwHid> res{};
#if SOUP_WINDOWS
		GUID HIDGuid;
#if true
		HidD_GetHidGuid(&HIDGuid);
#else
		HIDGuid.Data1 = 0x4D1E55B2;
		HIDGuid.Data2 = 0xf16f;
		HIDGuid.Data3 = 0x11cf;
		HIDGuid.Data4[0] = 0x88;
		HIDGuid.Data4[1] = 0xcb;
		HIDGuid.Data4[2] = 0x00;
		HIDGuid.Data4[3] = 0x11;
		HIDGuid.Data4[4] = 0x11;
		HIDGuid.Data4[5] = 0x00;
		HIDGuid.Data4[6] = 0x00;
		HIDGuid.Data4[7] = 0x30;
#endif

		wchar_t* device_interface_list = nullptr;

		// Could also use SetupDiEnumDeviceInterfaces & SetupDiGetDeviceInterfaceDetailW to iterate devices,
		// but allocating it all in one go is preferable.
		CONFIGRET cr;
		do
		{
			DWORD len;
			cr = CM_Get_Device_Interface_List_SizeW(&len, &HIDGuid, NULL, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
			if (cr != CR_SUCCESS)
			{
				return {};
			}

			if (device_interface_list != nullptr)
			{
				free(device_interface_list);
			}
			device_interface_list = (wchar_t*)calloc(len, sizeof(wchar_t));
			SOUP_ASSERT(device_interface_list != nullptr);

			cr = CM_Get_Device_Interface_ListW(&HIDGuid, NULL, device_interface_list, len, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
			if (cr != CR_SUCCESS && cr != CR_BUFFER_SMALL)
			{
				return {};
			}
		} while (cr == CR_BUFFER_SMALL);

		for (wchar_t* device_interface = device_interface_list; *device_interface; device_interface += wcslen(device_interface) + 1)
		{
			hwHid hid{};

			hid.handle = CreateFileW(
				device_interface,
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_OVERLAPPED,
				NULL
			);
			if (!hid.handle.isValid())
			{
				continue;
			}

			HIDD_ATTRIBUTES attrib;
			attrib.Size = sizeof(HIDD_ATTRIBUTES);
			if (HidD_GetAttributes(hid.handle, &attrib))
			{
				hid.vendor_id = attrib.VendorID;
				hid.product_id = attrib.ProductID;

				PHIDP_PREPARSED_DATA pp_data = nullptr;
				if (HidD_GetPreparsedData(hid.handle, &pp_data))
				{
					HIDP_CAPS caps;
					if (HidP_GetCaps(pp_data, &caps) == HIDP_STATUS_SUCCESS)
					{
						hid.usage = caps.Usage;
						hid.usage_page = caps.UsagePage;
						hid.output_report_byte_length = caps.OutputReportByteLength;
						hid.feature_report_byte_length = caps.FeatureReportByteLength;
						hid.read_buffer.reserve(caps.InputReportByteLength);

						res.emplace_back(std::move(hid));
					}
					HidD_FreePreparsedData(pp_data);
				}
			}
		}

		free(device_interface_list);
#elif SOUP_LINUX
		SharedLibrary libudev("libudev.so.1");
		SOUP_ASSERT(libudev.isLoaded(), "Failed to load libudev.so.1");

#define use_udev_func(name) const auto name = (name ## _t)libudev.getAddressMandatory(#name);

		use_udev_func(udev_new);
		use_udev_func(udev_unref);

		use_udev_func(udev_enumerate_unref);
		use_udev_func(udev_enumerate_new);
		use_udev_func(udev_enumerate_add_match_subsystem);
		use_udev_func(udev_enumerate_scan_devices);
		use_udev_func(udev_enumerate_get_list_entry);

		use_udev_func(udev_list_entry_get_next);
		use_udev_func(udev_list_entry_get_name);

		use_udev_func(udev_device_unref);
		use_udev_func(udev_device_new_from_syspath);
		use_udev_func(udev_device_get_devnode);
		use_udev_func(udev_device_get_parent_with_subsystem_devtype);
		use_udev_func(udev_device_get_sysattr_value);

		if (udev* udev = udev_new())
		{
			udev_enumerate* enumerate = udev_enumerate_new(udev);
			udev_enumerate_add_match_subsystem(enumerate, "hidraw");
			udev_enumerate_scan_devices(enumerate);

			udev_list_entry* devices;
			devices = udev_enumerate_get_list_entry(enumerate);

			udev_list_entry* entry;
			udev_list_entry_foreach(entry, devices)
			{
				const char* path = udev_list_entry_get_name(entry); // same as udev_device_get_syspath, /sys/devices/...

				hwHid hid{};
				if (parse_uevent_info(get_uevent_from_sysfs(path), hid.vendor_id, hid.product_id, hid.product_name, hid.serial_number))
				{
					hidraw_report_descriptor report_desc;
					if (get_hid_report_descriptor_from_sysfs(path, &report_desc) >= 0)
					{
						get_hid_usage(hid, report_desc.value, report_desc.size);

						udev_device* device = udev_device_new_from_syspath(udev, path);
						const char* dev_path = udev_device_get_devnode(device); // /dev/hidraw0 etc

						hid.handle = ::open(dev_path, O_RDWR | O_CLOEXEC);

						// The product string we got from `parse_uevent_info` is a bit bad because it is actually vendor string + product string.
						if (auto usb_dev = udev_device_get_parent_with_subsystem_devtype(device, "usb", "usb_device"))
						{
							hid.manufacturer_name = udev_device_get_sysattr_value(usb_dev, "manufacturer");
							hid.product_name = udev_device_get_sysattr_value(usb_dev, "product");
						}

						hid.read_buffer.reserve(1024); // We don't have a input_report_byte_length, so 1024 ought to be enough.

						res.emplace_back(std::move(hid));

						udev_device_unref(device);
					}
				}
			}

			udev_enumerate_unref(enumerate);
			udev_unref(udev);
		}
#endif
		return res;
	}

#if SOUP_WINDOWS
	std::string hwHid::getManufacturerName() const
	{
		std::string ret{};

		wchar_t buf[256];
		buf[0] = 0;
		if (HidD_GetManufacturerString(handle, buf, sizeof(buf)))
		{
			ret = unicode::utf16_to_utf8<std::wstring>(buf);
		}

		return ret;
	}

	std::string hwHid::getProductName() const
	{
		std::string ret{};

		wchar_t buf[256];
		buf[0] = 0;
		if (HidD_GetProductString(handle, buf, sizeof(buf)))
		{
			ret = unicode::utf16_to_utf8<std::wstring>(buf);
		}

		return ret;
	}

	std::string hwHid::getSerialNumber() const
	{
		std::string ret{};

		wchar_t buf[256];
		buf[0] = 0;
		if (HidD_GetSerialNumberString(handle, buf, sizeof(buf)))
		{
			ret = unicode::utf16_to_utf8<std::wstring>(buf);
		}

		return ret;
	}
#endif

	bool hwHid::hasReportId(uint8_t report_id) const noexcept
	{
		bool ret = false;
#if SOUP_WINDOWS
		PHIDP_PREPARSED_DATA pp_data = nullptr;
		if (HidD_GetPreparsedData(handle, &pp_data))
		{
			if (HidP_InitializeReportForID(HidP_Input, report_id, pp_data, (char*)read_buffer.data(), read_buffer.capacity()) == HIDP_STATUS_SUCCESS)
			{
				ret = true;
			}
			HidD_FreePreparsedData(pp_data);
		}
#endif
		return ret;
	}

	bool hwHid::hasReport()
	{
#if SOUP_WINDOWS
		if (!pending_read)
		{
			kickOffRead();
		}
		return HasOverlappedIoCompleted(&read_overlapped);
#elif SOUP_LINUX
		pollfd pfd;
		pfd.fd = handle;
		pfd.events = POLLIN;
		pfd.revents = 0;
		return poll(&pfd, 1, 0) != 0;
#else
		return true;
#endif
	}

	const Buffer& hwHid::receiveReport()
	{
		SOUP_ASSERT(havePermission(), "Attempt to read report from HID without having the needed permissions");
		read_buffer.resize(0);
#if SOUP_WINDOWS
		if (!pending_read)
		{
			kickOffRead();
		}
		if (GetOverlappedResult(handle, &read_overlapped, &bytes_read, TRUE))
		{
			read_buffer.resize(bytes_read);

			// Windows puts a report id at the front, but we want the raw data, so erasing it.
			if (bytes_read != 0
				&& read_buffer.at(0) == '\0'
				)
			{
				read_buffer.erase(0, 1);
			}
		}
		pending_read = false;
#elif SOUP_LINUX
		int bytes_read = ::read(handle, read_buffer.data(), read_buffer.capacity());
		if (bytes_read >= 0)
		{
			read_buffer.resize(bytes_read);
		}
#endif
		return read_buffer;
	}

	void hwHid::receiveFeatureReport(Buffer& buf) const
	{
#if SOUP_WINDOWS
		if (buf.size() < feature_report_byte_length)
		{
			buf.insert_back(feature_report_byte_length - buf.size(), '\0');
		}

		SOUP_ASSERT(HidD_GetFeature(handle, buf.data(), buf.size()));
#elif SOUP_LINUX
		// TODO
#endif
	}

	bool hwHid::sendReport(Buffer&& buf) const noexcept
	{
#if SOUP_WINDOWS
		// On Windows, the output report has to be at least as long as output_report_byte_length.
		if (buf.size() < output_report_byte_length)
		{
			buf.insert_back(output_report_byte_length - buf.size(), '\0');
		}

		OVERLAPPED overlapped{};
		DWORD bytesWritten;
		BOOL result = WriteFile(handle, buf.data(), buf.size(), &bytesWritten, &overlapped);
		if (result == FALSE
			&& GetLastError() == ERROR_IO_PENDING
			)
		{
			result = GetOverlappedResult(handle, &overlapped, &bytesWritten, TRUE);
		}
		return result && bytesWritten == buf.size();
#elif SOUP_LINUX
		// TODO
		return false;
#else
		return false;
#endif
	}

	bool hwHid::sendFeatureReport(Buffer&& buf) const noexcept
	{
#if SOUP_WINDOWS
		// On Windows, the feature report has to be at least as long as feature_report_byte_length.
		if (buf.size() < feature_report_byte_length)
		{
			buf.insert_back(feature_report_byte_length - buf.size(), '\0');
		}

		return HidD_SetFeature(handle, buf.data(), buf.size());
#elif SOUP_LINUX
		// TODO
		return false;
#else
		return false;
#endif
	}

#if SOUP_WINDOWS
	void hwHid::kickOffRead()
	{
		ReadFile(handle, read_buffer.data(), read_buffer.capacity(), &bytes_read, &read_overlapped);
		pending_read = true;
	}
#endif
}
