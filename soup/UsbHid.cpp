#include "UsbHid.hpp"

#if SOUP_WINDOWS

// Based on https://github.com/libusb/hidapi/blob/master/windows/hid.c
// Could also use SetupDiEnumDeviceInterfaces & SetupDiGetDeviceInterfaceDetailW to iterate devices,
// but allocating it all in one go is preferable.

#include <cfgmgr32.h>
#include <hidsdi.h>

#pragma comment(lib, "CfgMgr32.lib")
#pragma comment(lib, "hid.lib")

namespace soup
{
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
		);;
	}

	std::vector<UsbHid> UsbHid::getAll()
	{
		std::vector<UsbHid> res{};

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

						res.emplace_back(std::move(hid));
					}
					HidD_FreePreparsedData(pp_data);
				}
			}
		}

		free(device_interface_list);

		return res;
	}

	std::string UsbHid::pollReport() const
	{
		std::string buf(input_report_byte_length, '\0');
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
		return {};
	}
}

#endif
