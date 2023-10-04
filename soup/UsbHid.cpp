#include "UsbHid.hpp"

// Based on hidapi:
// - https://github.com/libusb/hidapi/blob/master/windows/hid.c
// - https://github.com/libusb/hidapi/blob/master/linux/hid.c

// If you want to use UsbHid on Linux, you need to `sudo apt install libudev-dev`
// and add `arg -DSOUP_HAVE_LIBUDEV_DEV` to Soup's .sun file.

#if SOUP_WINDOWS
#include <cfgmgr32.h>
#include <hidsdi.h>

#pragma comment(lib, "CfgMgr32.lib")
#pragma comment(lib, "hid.lib")

#include "unicode.hpp"
#elif defined(SOUP_HAVE_LIBUDEV_DEV)
#include <sys/stat.h> // open
#include <fcntl.h> // O_RDWR etc
#include <unistd.h> // read

#include <libudev.h>
#pragma comment(lib, "udev")

#include <linux/hidraw.h>

#include "string.hpp"
#endif

namespace soup
{
#if !SOUP_WINDOWS && defined(SOUP_HAVE_LIBUDEV_DEV)
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

				if (strcmp(key, "HID_ID") == 0) {
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
				else if (strcmp(key, "HID_NAME") == 0) {
					product_name_utf8 = value;
					found_name = true;
				}
				else if (strcmp(key, "HID_UNIQ") == 0) {
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

	[[nodiscard]] static uint16_t get_hid_usage(const uint8_t* report_descriptor, uint32_t size)
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

			if ((key & 0xfc) == 0x04)
			{
				return get_hid_report_bytes(report_descriptor, size, data_len, pos);
			}

			pos += data_len + key_size;
		}
		return 0;
	}
#endif

	std::vector<UsbHid> UsbHid::getAll()
	{
		std::vector<UsbHid> res{};
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
			UsbHid hid{};

			hid.handle = CreateFileW(
				device_interface,
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				0,
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
						hid.usage_page = caps.UsagePage;
						hid.input_report_byte_length = caps.InputReportByteLength;
						hid.output_report_byte_length = caps.OutputReportByteLength;
						hid.feature_report_byte_length = caps.FeatureReportByteLength;

						res.emplace_back(std::move(hid));
					}
					HidD_FreePreparsedData(pp_data);
				}
			}
		}

		free(device_interface_list);
#elif defined(SOUP_HAVE_LIBUDEV_DEV)
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

				UsbHid hid{};
				if (parse_uevent_info(get_uevent_from_sysfs(path), hid.vendor_id, hid.product_id, hid.product_name, hid.serial_number))
				{
					hidraw_report_descriptor report_desc;
					if (get_hid_report_descriptor_from_sysfs(path, &report_desc) >= 0)
					{
						hid.usage_page = get_hid_usage(report_desc.value, report_desc.size);
						if (hid.usage_page != 0)
						{
							udev_device* device = udev_device_new_from_syspath(udev, path);
							const char* dev_path = udev_device_get_devnode(device); // /dev/hidraw0 etc

							hid.handle = ::open(dev_path, O_RDWR | O_CLOEXEC);

							res.emplace_back(std::move(hid));

							udev_device_unref(device);
						}
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
	std::string UsbHid::getProductName() const
	{
		std::string ret{};

		wchar_t buf[256];
		if (HidD_GetProductString(handle, buf, sizeof(buf)))
		{
			ret = unicode::utf16_to_utf8<std::wstring>(buf);
		}

		return ret;
	}

	std::string UsbHid::getSerialNumber() const
	{
		std::string ret{};

		wchar_t buf[256];
		if (HidD_GetSerialNumberString(handle, buf, sizeof(buf)))
		{
			ret = unicode::utf16_to_utf8<std::wstring>(buf);
		}

		return ret;
	}
#endif

	Buffer UsbHid::pollReport() const
	{
		SOUP_ASSERT(havePermission());
#if SOUP_WINDOWS
		Buffer buf(input_report_byte_length);
		DWORD bytes_read;
		if (ReadFile(handle, buf.data(), input_report_byte_length, &bytes_read, NULL))
		{
			buf.resize(bytes_read);

			// Windows puts a report id at the front, but we want the raw data, so erasing it.
			if (bytes_read != 0
				//&& buf.at(0) == '\0'
				)
			{
				buf.erase(0, 1);
			}

			return buf;
		}
#elif defined(SOUP_HAVE_LIBUDEV_DEV)
		Buffer buf(1024); // We don't have a input_report_byte_length, so 1024 ought to be enough.
		int bytes_read = ::read(handle, buf.data(), buf.capacity());
		if (bytes_read >= 0)
		{
			buf.resize(bytes_read);
			return buf;
		}
#endif
		return {};
	}

	void UsbHid::sendReport(Buffer&& buf) const
	{
#if SOUP_WINDOWS
		// On Windows, the output report has to be at least as long as output_report_byte_length.
		if (buf.size() < output_report_byte_length)
		{
			buf.insert_back(output_report_byte_length - buf.size(), '\0');
		}

		DWORD bytesWritten;
		SOUP_ASSERT(WriteFile(handle, buf.data(), buf.size(), &bytesWritten, nullptr) && bytesWritten == buf.size());
#elif defined(SOUP_HAVE_LIBUDEV_DEV)
		// TODO
#endif
	}

	void UsbHid::sendFeatureReport(Buffer&& buf) const
	{
#if SOUP_WINDOWS
		// On Windows, the feature report has to be at least as long as feature_report_byte_length.
		if (buf.size() < feature_report_byte_length)
		{
			buf.insert_back(feature_report_byte_length - buf.size(), '\0');
		}

		SOUP_ASSERT(HidD_SetFeature(handle, buf.data(), buf.size()));
#elif defined(SOUP_HAVE_LIBUDEV_DEV)
		// TODO
#endif
	}
}
