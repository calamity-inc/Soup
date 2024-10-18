#include "HidReportDescriptor.hpp"

NAMESPACE_SOUP
{
	static uint32_t get_hid_report_bytes(const uint8_t* rawdesc, size_t size, uint32_t num_bytes, uint32_t pos)
	{
		if (pos + num_bytes < size)
		{
			if (num_bytes == 1)
			{
				return rawdesc[pos + 1];
			}
			else if (num_bytes == 2)
			{
				return (rawdesc[pos + 2] * 256 + rawdesc[pos + 1]);
			}
			else if (num_bytes == 4)
			{
				return (
					rawdesc[pos + 4] * 0x01000000 +
					rawdesc[pos + 3] * 0x00010000 +
					rawdesc[pos + 2] * 0x00000100 +
					rawdesc[pos + 1] * 0x00000001
					);
			}
		}
		return 0;
	}

	/*
	 * Gets the size of the HID item at the given position
	 * Returns 1 if successful, 0 if an invalid key
	 * Sets data_len and key_size when successful
	 */
	static bool get_hid_item_size(const uint8_t* rawdesc, size_t size, uint32_t pos, uint32_t* data_len, uint32_t* key_size)
	{
		const auto key = rawdesc[pos];

		/*
		 * This is a Long Item. The next byte contains the
		 * length of the data section (value) for this key.
		 * See the HID specification, version 1.11, section
		 * 6.2.2.3, titled "Long Items."
		 */
		if ((key & 0xf0) == 0xf0)
		{
			if (pos + 1 < size)
			{
				*data_len = rawdesc[pos + 1];
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
		const auto size_code = key & 0x3;
		switch (size_code)
		{
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
			SOUP_UNREACHABLE;
		};

		/* malformed report */
		return false;
	}

	HidReportDescriptor HidReportDescriptor::parse(const void* _rawdesc, size_t size)
	{
		const auto rawdesc = reinterpret_cast<const uint8_t*>(_rawdesc);

		HidReportDescriptor parsed;

		uint32_t report_size = 8;
		uint32_t report_count = 0;

		for (uint32_t pos = 0; pos < size; )
		{
			const auto key = rawdesc[pos];
			const auto key_cmd = key & 0xfc;

			uint32_t data_len, key_size;
			if (!get_hid_item_size(rawdesc, size, pos, &data_len, &key_size))
			{
				break;
			}

			//std::cout << std::hex << key_cmd << ": " << get_hid_report_bytes(rawdesc, size, data_len, pos) << std::endl;

			switch (key_cmd)
			{
			case 0x4: /* Usage Page 6.2.2.7 (Global) */
				parsed.usage_page = get_hid_report_bytes(rawdesc, size, data_len, pos);
				break;

			case 0x8: /* Usage 6.2.2.8 (Local) */
				if (data_len == 4) /* Usages 5.5 / Usage Page 6.2.2.7 */
				{
					parsed.usage_page = get_hid_report_bytes(rawdesc, size, 2, pos + 2);
					parsed.usage = get_hid_report_bytes(rawdesc, size, 2, pos);
				}
				else
				{
					parsed.usage = get_hid_report_bytes(rawdesc, size, data_len, pos);
				}
				break;

			case 0x74: // Report Size
				report_size = get_hid_report_bytes(rawdesc, size, data_len, pos);
				break;

			case 0x94: // Report count
				report_count = get_hid_report_bytes(rawdesc, size, data_len, pos);
				break;

			case 0x80: // Input
				parsed.input_report_byte_length = ((report_size * report_count) / 8) + 1;
				break;

			case 0x90: // Output
				parsed.output_report_byte_length = ((report_size * report_count) / 8) + 1;
				break;

			case 0xB0: // Feature
				parsed.feature_report_byte_length = ((report_size * report_count) / 8) + 1;
				break;
			}

			pos += data_len + key_size;
		}

		return parsed;
	}
}
