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
		bool is_bluetooth;
		bool disconnected = false;

		hwGamepad(const char* name, hwHid&& hid, bool is_bluetooth = false)
			: name(name), hid(std::move(hid)), is_bluetooth(is_bluetooth)
		{
		}

	public:
		[[nodiscard]] static std::vector<hwGamepad> getAll();

		// Some gamepads pretty much always have something to report,
		// but the assumption should be that this will block until the user causes a change.
		// You can use `hid.hasReport()` to check if this will block.
		[[nodiscard]] Status receiveStatus();

		[[nodiscard]] bool hasInvertedActionButtons() const noexcept { return false; }

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
	};
}
