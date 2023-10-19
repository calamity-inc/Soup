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
					res.emplace_back(hwGamepad("DualShock 4", std::move(hid)));
				}
				else if (hid.vendor_id == 0x18d1 // Google
					&& hid.product_id == 0x9400 // Stadia Controller
					)
				{
					Buffer buf;
					buf.push_back(0x05);
					bool is_bluetooth = !hid.sendReport(std::move(buf));
					res.emplace_back(hwGamepad("Stadia Controller", std::move(hid), is_bluetooth));
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

	SOUP_PACKET(StadiaReport)
	{
		u8 dpad;
		u8 buttons_1; // 01 = capture, 02 = assistant, 04 = LT, 08 = RT, 10 = meta, 20 = menu, 40 = options, 80 = RS
		u8 buttons_2; // 01 = LS, 02 = RB, 04 = LB, 08 = Y, 10 = X, 20 = B, 40 = A
		u8 left_stick_x;
		u8 left_stick_y;
		u8 right_stick_x;
		u8 right_stick_y;
		u8 left_trigger;
		u8 right_trigger;

		SOUP_PACKET_IO(s)
		{
			return s.skip(1) // report id (03)
				&& s.u8(dpad)
				&& s.u8(buttons_1)
				&& s.u8(buttons_2)
				&& s.u8(left_stick_x)
				&& s.u8(left_stick_y)
				&& s.u8(right_stick_x)
				&& s.u8(right_stick_y)
				&& s.u8(left_trigger)
				&& s.u8(right_trigger)
				;
		}
	};

	Status hwGamepad::receiveStatus()
	{
		update();

		Status status{};

		const Buffer& report_data = hid.receiveReport();
		SOUP_IF_UNLIKELY (report_data.empty())
		{
			disconnected = true;
		}
		else
		{
			BufferRefReader r(report_data);

			if (hid.vendor_id == 0x54c) // DS4
			{
				if (report_data.at(0) == 0x11)
				{
					r.skip(3); // Bluetooth report starts with 11 C0 00
					is_bluetooth = true;
				}
				else
				{
					r.skip(1); // USB report starts with 05
					is_bluetooth = false;
				}

				Ds4Report report;
				report.read(r);

				status.left_stick_x = (static_cast<float>(report.left_stick_x) / 255.0f);
				status.left_stick_y = (static_cast<float>(report.left_stick_y) / 255.0f);
				status.right_stick_x = (static_cast<float>(report.right_stick_x) / 255.0f);
				status.right_stick_y = (static_cast<float>(report.right_stick_y) / 255.0f);
				status.left_trigger = (static_cast<float>(report.l2) / 255.0f);
				status.right_trigger = (static_cast<float>(report.r2) / 255.0f);
				status.setDpad(report.buttons_1 & 0xf);
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
			}
			else // Stadia Controller
			{
				StadiaReport report;
				report.read(r);

				// The sticks' min value seems to be 1
				if (report.left_stick_x != 0) { report.left_stick_x -= 1; }
				if (report.left_stick_y != 0) { report.left_stick_y -= 1; }
				if (report.right_stick_x != 0) { report.right_stick_x -= 1; }
				if (report.right_stick_y != 0) { report.right_stick_y -= 1; }
				status.left_stick_x = (static_cast<float>(report.left_stick_x) / 254.0f);
				status.left_stick_y = (static_cast<float>(report.left_stick_y) / 254.0f);
				status.right_stick_x = (static_cast<float>(report.right_stick_x) / 254.0f);
				status.right_stick_y = (static_cast<float>(report.right_stick_y) / 254.0f);

				status.left_trigger = (static_cast<float>(report.left_trigger) / 255.0f);
				status.right_trigger = (static_cast<float>(report.right_trigger) / 255.0f);

				status.setDpad(report.dpad);
				status.buttons[BTN_SHARE] = (report.buttons_1 & 0x01);
				status.buttons[BTN_ASSISTANT] = (report.buttons_1 & 0x02);
				status.buttons[BTN_LTRIGGER] = (report.buttons_1 & 0x04);
				status.buttons[BTN_RTRIGGER] = (report.buttons_1 & 0x08);
				status.buttons[BTN_META] = (report.buttons_1 & 0x10);
				status.buttons[BTN_MENU] = (report.buttons_1 & 0x20);
				status.buttons[BTN_OPTIONS] = (report.buttons_1 & 0x40);
				status.buttons[BTN_RSTICK] = (report.buttons_1 & 0x80);
				status.buttons[BTN_LSTICK] = (report.buttons_2 & 0x01);
				status.buttons[BTN_RBUMPER] = (report.buttons_2 & 0x02);
				status.buttons[BTN_LBUMPER] = (report.buttons_2 & 0x04);
				status.buttons[BTN_ACT_LEFT] = (report.buttons_2 & 0x08);
				status.buttons[BTN_ACT_UP] = (report.buttons_2 & 0x10);
				status.buttons[BTN_ACT_RIGHT] = (report.buttons_2 & 0x20);
				status.buttons[BTN_ACT_DOWN] = (report.buttons_2 & 0x40);
			}
		}

		return status;
	}

	// Should maybe be called "has programmable light" because Stadia Controller does have a light, we just can't modify it...
	bool hwGamepad::hasLight() const noexcept
	{
		return hid.vendor_id == 0x54c; // DS4
	}

	void hwGamepad::setLight(Rgb colour)
	{
		if (hasLight()) // no point in sending a no-op report
		{
			this->colour = colour;
			sendReport();
		}
	}

	bool hwGamepad::canRumble() const noexcept
	{
		if (hid.vendor_id == 0x18d1) // Stadia Controller; rumble only works via USB
		{
			return !is_bluetooth;
		}
		return true;
	}

	void hwGamepad::rumbleWeak(uint8_t intensity, time_t ms)
	{
		this->weak_rumble_intensity = intensity;
		this->weak_rumble_until = time::millis() + ms;
		sendReport();
	}

	void hwGamepad::rumbleStrong(uint8_t intensity, time_t ms)
	{
		this->strong_rumble_intensity = intensity;
		this->strong_rumble_until = time::millis() + ms;
		sendReport();
	}

	void hwGamepad::update()
	{
		if (weak_rumble_intensity != 0
			&& time::millisUntil(weak_rumble_until) <= 0
			)
		{
			sendReport();
		}
		else if (strong_rumble_intensity != 0
			&& time::millisUntil(strong_rumble_until) <= 0
			)
		{
			sendReport();
		}
	}

	void hwGamepad::sendReport()
	{
		if (weak_rumble_intensity != 0
			&& time::millisUntil(weak_rumble_until) <= 0
			)
		{
			weak_rumble_intensity = 0;
		}
		if (strong_rumble_intensity != 0
			&& time::millisUntil(strong_rumble_until) <= 0
			)
		{
			strong_rumble_intensity = 0;
		}

		if (hid.vendor_id == 0x54c) // DS4
		{
			Buffer buf(11);
			buf.push_back(0x07);
			buf.push_back(0x00);
			buf.push_back(0x00);
			buf.push_back(weak_rumble_intensity);
			buf.push_back(strong_rumble_intensity);
			buf.push_back(colour.r);
			buf.push_back(colour.g);
			buf.push_back(colour.b);
			buf.push_back(0x00);
			buf.push_back(0x00);
			sendReportDs4(std::move(buf));
		}
		else // Stadia Controller
		{
			Buffer buf(5);
			buf.push_back(0x05);
			buf.push_back(strong_rumble_intensity);
			buf.push_back(strong_rumble_intensity);
			buf.push_back(weak_rumble_intensity);
			buf.push_back(weak_rumble_intensity);
			hid.sendReport(std::move(buf));
		}
	}

	void hwGamepad::sendReportDs4(Buffer&& buf) const
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

	void hwGamepad::Status::setDpad(uint8_t dpad)
	{
		switch (dpad)
		{
		case 0:
			buttons[BTN_DPAD_UP] = true;
			break;

		case 1:
			buttons[BTN_DPAD_UP] = true;
			buttons[BTN_DPAD_RIGHT] = true;
			break;

		case 2:
			buttons[BTN_DPAD_RIGHT] = true;
			break;

		case 3:
			buttons[BTN_DPAD_RIGHT] = true;
			buttons[BTN_DPAD_DOWN] = true;
			break;

		case 4:
			buttons[BTN_DPAD_DOWN] = true;
			break;

		case 5:
			buttons[BTN_DPAD_DOWN] = true;
			buttons[BTN_DPAD_LEFT] = true;
			break;

		case 6:
			buttons[BTN_DPAD_LEFT] = true;
			break;

		case 7:
			buttons[BTN_DPAD_LEFT] = true;
			buttons[BTN_DPAD_UP] = true;
			break;
		}
	}
}
