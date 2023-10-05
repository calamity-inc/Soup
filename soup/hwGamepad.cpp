#include "hwGamepad.hpp"

#include "BufferRefReader.hpp"
#include "crc32.hpp"
#include "Packet.hpp"
#include "time.hpp"

// https://gitlab.com/CalcProgrammer1/OpenRGB/-/blob/master/Controllers/SonyGamepadController/SonyDS4Controller.cpp?ref_type=heads
// http://eleccelerator.com/wiki/index.php?title=DualShock_4

namespace soup
{
	using Status = hwGamepad::Status;

	std::vector<hwGamepad> hwGamepad::getAll()
	{
		std::vector<hwGamepad> res{};
		for (auto& hid : hwHid::getAll())
		{
			if (hid.usage == 0x05)
			{
				if (hid.vendor_id == 0x54c)
				{
					res.emplace_back(hwGamepad{ "DualShock 4", std::move(hid)});
				}
			}
		}
		return res;
	}

	// https://www.psdevwiki.com/ps4/DS4-USB#Data_Format
	SOUP_PACKET(Ds4Report)
	{
		u8 left_stick_x; // 1
		u8 left_stick_y; // 2
		u8 right_stick_x; // 3
		u8 right_stick_y; // 4
		u8 buttons_1; // 5
		u8 buttons_2; // 6
		u8 buttons_3_and_counter; // 7
		u8 l2; // 8
		u8 r2; // 9
		//u16 timestamp; // 10-11
		//u8 battery; // 12
		//i16 gyro_x; // 13-14
		//i16 gyro_y; // 15-16
		//i16 gyro_z; // 17-18
		//i16 accel_x; // 19-20
		//i16 accel_y; // 21-22
		//i16 accel_z; // 23-24

		SOUP_PACKET_IO(s)
		{
			return s.u8(left_stick_x)
				&& s.u8(left_stick_y)
				&& s.u8(right_stick_x)
				&& s.u8(right_stick_y)
				&& s.u8(buttons_1)
				&& s.u8(buttons_2)
				&& s.u8(buttons_3_and_counter)
				&& s.u8(l2)
				&& s.u8(r2)
				//&& s.u16(timestamp)
				//&& s.u8(battery)
				//&& s.i16(gyro_x)
				//&& s.i16(gyro_y)
				//&& s.i16(gyro_z)
				//&& s.i16(accel_x)
				//&& s.i16(accel_y)
				//&& s.i16(accel_z)
				;
		}
	};

	Status hwGamepad::pollStatus()
	{
		update();

		auto report_data = hid.pollReport();
		if (report_data.at(0) == 0x11)
		{
			report_data.erase(0, 3); // Bluetooth report starts with 11 C0 00
			is_bluetooth = true;
		}
		else
		{
			report_data.erase(0, 1); // USB report starts with 05
			is_bluetooth = false;
		}
		BufferRefReader r(report_data);
		Ds4Report report;
		report.read(r);

		Status status{};
		status.left_stick_x = (static_cast<float>(report.left_stick_x) / 255.0f);
		status.left_stick_y = (static_cast<float>(report.left_stick_y) / 255.0f);
		status.right_stick_x = (static_cast<float>(report.right_stick_x) / 255.0f);
		status.right_stick_y = (static_cast<float>(report.right_stick_y) / 255.0f);
		status.left_trigger = (static_cast<float>(report.l2) / 255.0f);
		status.right_trigger = (static_cast<float>(report.r2) / 255.0f);
		switch (report.buttons_1 & 0xf)
		{
		case 0:
			status.buttons[BTN_DPAD_UP] = true;
			break;

		case 1:
			status.buttons[BTN_DPAD_UP] = true;
			status.buttons[BTN_DPAD_RIGHT] = true;
			break;

		case 2:
			status.buttons[BTN_DPAD_RIGHT] = true;
			break;

		case 3:
			status.buttons[BTN_DPAD_RIGHT] = true;
			status.buttons[BTN_DPAD_DOWN] = true;
			break;

		case 4:
			status.buttons[BTN_DPAD_DOWN] = true;
			break;

		case 5:
			status.buttons[BTN_DPAD_DOWN] = true;
			status.buttons[BTN_DPAD_LEFT] = true;
			break;

		case 6:
			status.buttons[BTN_DPAD_LEFT] = true;
			break;

		case 7:
			status.buttons[BTN_DPAD_LEFT] = true;
			status.buttons[BTN_DPAD_UP] = true;
			break;
		}
		status.buttons[BTN_ACT_LEFT] = (report.buttons_1 & (1 << 4));
		status.buttons[BTN_ACT_DOWN] = (report.buttons_1 & (1 << 5));
		status.buttons[BTN_ACT_RIGHT] = (report.buttons_1 & (1 << 6));
		status.buttons[BTN_ACT_UP] = (report.buttons_1 & (1 << 7));
		status.buttons[BTN_LBUMPER] = (report.buttons_2 & (1 << 0));
		status.buttons[BTN_RBUMPER] = (report.buttons_2 & (1 << 1));
		status.buttons[BTN_LTRIGGER] = (report.buttons_2 & (1 << 2));
		status.buttons[BTN_RTRIGGER] = (report.buttons_2 & (1 << 3));
		status.buttons[BTN_SHARE] = (report.buttons_2 & (1 << 4));
		status.buttons[BTN_OPTIONS] = (report.buttons_2 & (1 << 5));
		status.buttons[BTN_LSTICK] = (report.buttons_2 & (1 << 6));
		status.buttons[BTN_RSTICK] = (report.buttons_2 & (1 << 7));
		status.buttons[BTN_META] = (report.buttons_3_and_counter & (1 << 0));
		status.buttons[BTN_TOUCHPAD] = (report.buttons_3_and_counter & (1 << 1));
		return status;
	}

	void hwGamepad::setLight(Rgb colour)
	{
		this->colour = colour;
		sendReport();
	}

	void hwGamepad::rumbleWeak(time_t ms)
	{
		this->weak_rumble_until = time::millis() + ms;
		sendReport();
	}

	void hwGamepad::rumbleStrong(time_t ms)
	{
		this->strong_rumble_until = time::millis() + ms;
		sendReport();
	}

	void hwGamepad::update()
	{
		if (weak_rumble_until != 0
			&& time::millisUntil(weak_rumble_until) <= 0
			)
		{
			sendReport();
		}
		else if (strong_rumble_until != 0
			&& time::millisUntil(strong_rumble_until) <= 0
			)
		{
			sendReport();
		}
	}

	void hwGamepad::sendReport()
	{
		if (weak_rumble_until != 0
			&& time::millisUntil(weak_rumble_until) <= 0
			)
		{
			weak_rumble_until = 0;
		}
		if (strong_rumble_until != 0
			&& time::millisUntil(strong_rumble_until) <= 0
			)
		{
			strong_rumble_until = 0;
		}

		Buffer buf(11);
		buf.push_back(0x07);
		buf.push_back(0x00);
		buf.push_back(0x00);
		buf.push_back(weak_rumble_until ? 0xff : 0x00);
		buf.push_back(strong_rumble_until ? 0xff : 0x00);
		buf.push_back(colour.r);
		buf.push_back(colour.g);
		buf.push_back(colour.b);
		buf.push_back(0x00);
		buf.push_back(0x00);
		sendReport(std::move(buf));
	}

	void hwGamepad::sendReport(Buffer&& buf) const
	{
		if (!is_bluetooth)
		{
			// Start data with 05
			buf.insert_front(1, 0x05);
		}
		else
		{
			// Start data with A2 11 C0 00
			buf.insert_front(1, 0x00);
			buf.insert_front(1, 0xC0);
			buf.insert_front(1, 0x11);
			buf.insert_front(1, 0xA2);

			buf.insert_back(75 - buf.size(), 0);
			uint32_t crc = crc32::hash(buf.data(), 75);
			buf.erase(0, 1); // hash needs to include the 0xA2, but we don't want to send that
			buf.push_back((0x000000FF & crc));
			buf.push_back((0x0000FF00 & crc) >> 8);
			buf.push_back((0x00FF0000 & crc) >> 16);
			buf.push_back((0xFF000000 & crc) >> 24);
		}
		hid.sendReport(std::move(buf));
	}
}
