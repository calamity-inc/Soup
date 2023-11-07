#include "cli.hpp"

#include "base.hpp"

#if SOUP_WINDOWS
#include <iostream>
#include <thread>

#include <aud_maths.hpp>
#include <audDevice.hpp>
#include <audMixer.hpp>
#include <audNote.hpp>
#include <audPlayback.hpp>
#include <audSound.hpp>
#include <DigitalKeyboard.hpp>
#include <Key.hpp>
#include <UniquePtr.hpp>

using namespace soup;

struct KeyboardPiano : public audSound
{
	DigitalKeyboard keyboard;

	[[nodiscard]] double getAmplitude(double t) final
	{
		keyboard.update();
		if (keyboard.keys[KEY_Z])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::C4)) * t);
		}
		if (keyboard.keys[KEY_S])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::CSHARP4)) * t);
		}
		if (keyboard.keys[KEY_X])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::D4)) * t);
		}
		if (keyboard.keys[KEY_C])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::E4)) * t);
		}
		if (keyboard.keys[KEY_V])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::F4)) * t);
		}
		if (keyboard.keys[KEY_G])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::FSHARP4)) * t);
		}
		if (keyboard.keys[KEY_B])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::G4)) * t);
		}
		if (keyboard.keys[KEY_H])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::GSHARP4)) * t);
		}
		if (keyboard.keys[KEY_N])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::A4)) * t);
		}
		if (keyboard.keys[KEY_J])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::ASHARP4)) * t);
		}
		if (keyboard.keys[KEY_M])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::B4)) * t);
		}
		if (keyboard.keys[KEY_COMMA])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::C5)) * t);
		}
		if (keyboard.keys[KEY_L])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::CSHARP5)) * t);
		}
		if (keyboard.keys[KEY_PERIOD])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::D5)) * t);
		}
		if (keyboard.keys[KEY_SEMICOLON])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::DSHARP5)) * t);
		}
		if (keyboard.keys[KEY_SLASH])
		{
			return sin(HZ_TO_ANGVEL(audNoteToHz(audNote::E5)) * t);
		}
		return 0;
	}
};
#endif

void cli_midikeyboard()
{
#if SOUP_WINDOWS
	auto dev = audDevice::getDefault();
	std::cout << "Playing on " << dev.getName() << "\n";
	auto pb = dev.open();
	audMixer mix;
	mix.setOutput(*pb);
	auto piano = soup::make_shared<KeyboardPiano>();
	mix.playSound(piano);
	std::this_thread::sleep_for(std::chrono::years(INT_MAX));
#else
	std::cout << "Sorry, this only works on Windows (for now).\n";
#endif
}
