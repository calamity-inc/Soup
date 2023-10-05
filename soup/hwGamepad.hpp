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
		};

		const char* name;
		hwHid hid;
		bool is_bluetooth;

		[[nodiscard]] static std::vector<hwGamepad> getAll();

		[[nodiscard]] Status pollStatus();

		[[nodiscard]] bool hasLight() const noexcept { return true; }
		[[nodiscard]] bool hasGyro() const noexcept { return true; }
		[[nodiscard]] bool hasAccel() const noexcept { return true; }

		Rgb colour = Rgb::BLACK;
		void setLight(Rgb colour);

		time_t weak_rumble_until = 0;
		time_t strong_rumble_until = 0;
		void rumbleWeak(time_t ms);
		void rumbleStrong(time_t ms);

		void update();
	private:
		void sendReport();
		void sendReport(Buffer&& buf) const;
	};
}
