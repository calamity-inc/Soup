#pragma once

#include "hwHid.hpp"

#include "buttons.hpp"
#include "Rgb.hpp"

namespace soup
{
	class hwGamepad
	{
	public:
		struct Status
		{
			float left_stick_x;
			float left_stick_y;
			float right_stick_x;
			float right_stick_y;
			float left_trigger;
			float right_trigger;
			bool buttons[NUM_BUTTONS];

			void setDpad(uint8_t dpad);
		};

		const char* name;
		hwHid hid;
		bool disconnected = false;
	protected:
		union
		{
			struct
			{
				bool is_bluetooth;
			} ds4, stadia;
			struct
			{
				bool has_calibration_data;
				uint16_t left_stick_x_min;
				uint16_t left_stick_x_max_minus_min;
				uint16_t left_stick_y_min;
				uint16_t left_stick_y_max_minus_min;
				uint16_t right_stick_x_min;
				uint16_t right_stick_x_max_minus_min;
				uint16_t right_stick_y_min;
				uint16_t right_stick_y_max_minus_min;
			} switch_pro;
		};

		hwGamepad(const char* name, hwHid&& hid)
			: name(name), hid(std::move(hid))
		{
		}

	public:
		[[nodiscard]] static std::vector<hwGamepad> getAll();

		// Some gamepads pretty much always have something to report,
		// but the assumption should be that this will block until the user causes a change.
		// You can use `hid.hasReport()` to check if this will block.
		[[nodiscard]] Status receiveStatus();

		[[nodiscard]] bool hasAnalogueTriggers() const noexcept;
		[[nodiscard]] bool hasInvertedActionButtons() const noexcept;

	protected:
		Rgb colour = Rgb::BLACK;
	public:
		[[nodiscard]] bool hasLight() const noexcept;
		void setLight(Rgb colour);

		uint8_t weak_rumble_intensity = 0;
		time_t weak_rumble_until = 0;
		uint8_t strong_rumble_intensity = 0;
		time_t strong_rumble_until = 0;
		[[nodiscard]] bool canRumble() const noexcept;
		void rumbleWeak(uint8_t intensity, time_t ms);
		void rumbleStrong(uint8_t intensity, time_t ms);

		void update();
	private:
		void sendReport();
		void sendReportDs4(Buffer&& buf) const;

		void calibrateSwitchProController();
	};
}
