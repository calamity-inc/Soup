#pragma once

#include "kbRgb.hpp"

#include "hwHid.hpp"

NAMESPACE_SOUP
{
	class kbRgbWooting final : public kbRgb
	{
	public:
		hwHid hid;
		uint8_t columns;
		bool small_report = false;
	protected:
		bool inited = false;

	public:
		kbRgbWooting(const char* name, uint8_t columns, bool arm_based, hwHid&& hid)
			: kbRgb(name), hid(std::move(hid)), columns(columns)
		{
			if (arm_based && hid.input_report_byte_length != 257)
			{
				small_report = true;
			}
		}

		~kbRgbWooting() final;

		[[nodiscard]] bool controlsDevice(const hwHid& hid) const noexcept final;

		void deinit() final;

		void setKey(Key key, Rgb colour) final;
	protected:
		void setKeyImpl(uint8_t wk, Rgb colour);
		
	public:
		void setKeys(const Rgb(&colours)[NUM_KEYS]) final;

		[[nodiscard]] uint8_t getNumColumns() const noexcept final;
		[[nodiscard]] Key mapPosToKey(uint8_t row, uint8_t column) const noexcept final;

		[[nodiscard]] float getBrightness();

		static void mapCanvasToKeys(Rgb(&keys)[NUM_KEYS], const Canvas& c);

		[[nodiscard]] bool isUwu() const noexcept;

		enum UwuLed : std::underlying_type_t<Key>
		{
			UWULED_TOP_1 = KEY_ESCAPE,
			UWULED_TOP_2 = KEY_F1,
			UWULED_TOP_3 = KEY_F3,
			UWULED_TOP_4 = KEY_F5,
			UWULED_LEFT_1 = KEY_BACKQUOTE,
			UWULED_LEFT_2 = KEY_TAB,
			UWULED_LEFT_3 = KEY_CAPS_LOCK,
			UWULED_RIGHT_1 = KEY_6,
			UWULED_RIGHT_2 = KEY_Y,
			UWULED_RIGHT_3 = KEY_H,
			UWULED_BOTTOM_1 = KEY_INTL_BACKSLASH,
			UWULED_BOTTOM_2 = KEY_Z,
			UWULED_BOTTOM_3 = KEY_C,
			UWULED_BOTTOM_4 = KEY_V,
			UWULED_KEY_1 = KEY_Q,
			UWULED_KEY_2 = KEY_E,
			UWULED_KEY_3 = KEY_T,
		};
	};
}
