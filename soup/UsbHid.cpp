#include "UsbHid.hpp"

#if SOUP_WINDOWS
// Based on https://github.com/libusb/hidapi/blob/master/windows/hid.c
// Could also use SetupDiEnumDeviceInterfaces & SetupDiGetDeviceInterfaceDetailW to iterate devices,
// but allocating it all in one go is preferable.

#include <cfgmgr32.h>
#include <hidsdi.h>

#pragma comment(lib, "CfgMgr32.lib")
#pragma comment(lib, "hid.lib")

#include "unicode.hpp"
#endif

namespace soup
{
#if SOUP_WINDOWS
	[[nodiscard]] static HANDLE open_device(const wchar_t* path)
	{
		return CreateFileW(
			path,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);
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

			hid.handle = open_device(device_interface);
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
#endif
		return res;
	}

	std::string UsbHid::getProductName() const
	{
		std::string ret{};
#if SOUP_WINDOWS
		wchar_t buf[256];
		if (HidD_GetProductString(handle, buf, sizeof(buf)))
		{
			ret = unicode::utf16_to_utf8<std::wstring>(buf);
		}
#endif
		return ret;
	}

	std::string UsbHid::getSerialNumber() const
	{
		std::string ret{};
#if SOUP_WINDOWS
		wchar_t buf[256];
		if (HidD_GetSerialNumberString(handle, buf, sizeof(buf)))
		{
			ret = unicode::utf16_to_utf8<std::wstring>(buf);
		}
#endif
		return ret;
	}

	Buffer UsbHid::pollReport() const
	{
#if SOUP_WINDOWS
		Buffer buf;
		buf.insert_back(input_report_byte_length, '\0');
		DWORD bytesRead;
		if (ReadFile(handle, buf.data(), input_report_byte_length, &bytesRead, NULL))
		{
			buf.resize(bytesRead);

			// Windows puts a report id at the front, but we want the raw data, so erasing it.
			if (bytesRead != 0
				//&& buf.at(0) == '\0'
				)
			{
				buf.erase(0, 1);
			}
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
#endif
	}
}
