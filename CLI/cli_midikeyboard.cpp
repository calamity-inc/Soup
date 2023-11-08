#include "cli.hpp"

#include "base.hpp"

#include <iostream>

#if SOUP_WINDOWS
#include <aud_common.hpp>
#include <audDevice.hpp>
#include <audMixer.hpp>
#include <audNote.hpp>
#include <audNoteEnvelope.hpp>
#include <audPlayback.hpp>
#include <DigitalKeyboard.hpp>
#include <Key.hpp>
#include <UniquePtr.hpp>
#endif

using namespace soup;

#if SOUP_WINDOWS
static void maintainNote(audMixer& mix, SharedPtr<audNoteEnvelope>* sounds, audNote note, bool should_be_playing)
{
	if (should_be_playing)
	{
		if (!sounds[static_cast<size_t>(note)])
		{
			auto sound = soup::make_shared<audNoteEnvelope>(note);
			sounds[static_cast<size_t>(note)] = sound;
			mix.playSound(std::move(sound));
		}
	}
	else
	{
		if (sounds[static_cast<size_t>(note)])
		{
			sounds[static_cast<size_t>(note)]->released = true;
			sounds[static_cast<size_t>(note)].reset();
		}
	}
}
#endif

void cli_midikeyboard()
{
#if SOUP_WINDOWS
	auto dev = audDevice::getDefault();
	std::cout << "Playing on " << dev.getName() << "\n";
	auto pb = dev.open();
	audMixer mix;
	mix.setOutput(*pb);
	DigitalKeyboard keyboard;
	SharedPtr<audNoteEnvelope> sounds[static_cast<size_t>(audNote::SIZE)];
	while (true)
	{
		keyboard.update();
		maintainNote(mix, sounds, audNote::C4, keyboard.keys[KEY_Z]);
		maintainNote(mix, sounds, audNote::CSHARP4, keyboard.keys[KEY_S]);
		maintainNote(mix, sounds, audNote::D4, keyboard.keys[KEY_X]);
		maintainNote(mix, sounds, audNote::E4, keyboard.keys[KEY_C]);
		maintainNote(mix, sounds, audNote::F4, keyboard.keys[KEY_V]);
		maintainNote(mix, sounds, audNote::FSHARP4, keyboard.keys[KEY_G]);
		maintainNote(mix, sounds, audNote::G4, keyboard.keys[KEY_B]);
		maintainNote(mix, sounds, audNote::GSHARP4, keyboard.keys[KEY_H]);
		maintainNote(mix, sounds, audNote::A4, keyboard.keys[KEY_N]);
		maintainNote(mix, sounds, audNote::ASHARP4, keyboard.keys[KEY_J]);
		maintainNote(mix, sounds, audNote::B4, keyboard.keys[KEY_M]);
		maintainNote(mix, sounds, audNote::C5, keyboard.keys[KEY_COMMA]);
		maintainNote(mix, sounds, audNote::CSHARP5, keyboard.keys[KEY_L]);
		maintainNote(mix, sounds, audNote::D5, keyboard.keys[KEY_PERIOD]);
		maintainNote(mix, sounds, audNote::DSHARP5, keyboard.keys[KEY_SEMICOLON]);
		maintainNote(mix, sounds, audNote::E5, keyboard.keys[KEY_SLASH]);
	}
#else
	std::cout << "Sorry, this only works on Windows (for now).\n";
#endif
}
