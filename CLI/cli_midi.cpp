#include "cli.hpp"

#include "base.hpp"

#include <iostream>
#include <thread>

#if SOUP_WINDOWS || SOUP_LINUX
#include <aud_common.hpp>
#include <audDevice.hpp>
#include <audMixer.hpp>
#include <audNote.hpp>
#include <audNoteEnvelope.hpp>
#include <audPlayback.hpp>
#include <DigitalKeyboard.hpp>
#include <FileReader.hpp>
#include <Key.hpp>
#include <MidiComposition.hpp>
#include <string.hpp>
#endif

using namespace soup;

#if SOUP_WINDOWS || SOUP_LINUX
static void maintainNote(audMixer& mix, SharedPtr<audNoteEnvelope>(&sounds)[AUDNOTE_SIZE], audNote_t note, bool should_be_playing)
{
	if (should_be_playing)
	{
		if (!sounds[note])
		{
			auto sound = soup::make_shared<audNoteEnvelope>(note);
			sounds[note] = sound;
			mix.playSound(std::move(sound));
		}
	}
	else
	{
		if (sounds[note])
		{
			sounds[note]->released = true;
			sounds[note].reset();
		}
	}
}
#endif

void cli_midi(int argc, const char** argv)
{
#if SOUP_WINDOWS || SOUP_LINUX
	if (argc > 0)
	{
		std::string subcommand = argv[0];
		string::lower(subcommand);

		if (subcommand == "keyboard")
		{
#if SOUP_WINDOWS
			auto dev = audDevice::getDefault();
			std::cout << "Playing on " << dev.getName() << std::endl;
			auto pb = dev.open();
			audMixer mix;
			mix.setOutput(*pb);
			SharedPtr<audNoteEnvelope> sounds[AUDNOTE_SIZE];

			DigitalKeyboard keyboard;
			while (true)
			{
				keyboard.update();
				maintainNote(mix, sounds, AUDNOTE_C4, keyboard.keys[KEY_Z]);
				maintainNote(mix, sounds, AUDNOTE_CSHARP4, keyboard.keys[KEY_S]);
				maintainNote(mix, sounds, AUDNOTE_D4, keyboard.keys[KEY_X]);
				maintainNote(mix, sounds, AUDNOTE_E4, keyboard.keys[KEY_C]);
				maintainNote(mix, sounds, AUDNOTE_F4, keyboard.keys[KEY_V]);
				maintainNote(mix, sounds, AUDNOTE_FSHARP4, keyboard.keys[KEY_G]);
				maintainNote(mix, sounds, AUDNOTE_G4, keyboard.keys[KEY_B]);
				maintainNote(mix, sounds, AUDNOTE_GSHARP4, keyboard.keys[KEY_H]);
				maintainNote(mix, sounds, AUDNOTE_A4, keyboard.keys[KEY_N]);
				maintainNote(mix, sounds, AUDNOTE_ASHARP4, keyboard.keys[KEY_J]);
				maintainNote(mix, sounds, AUDNOTE_B4, keyboard.keys[KEY_M]);
				maintainNote(mix, sounds, AUDNOTE_C5, keyboard.keys[KEY_COMMA]);
				maintainNote(mix, sounds, AUDNOTE_CSHARP5, keyboard.keys[KEY_L]);
				maintainNote(mix, sounds, AUDNOTE_D5, keyboard.keys[KEY_PERIOD]);
				maintainNote(mix, sounds, AUDNOTE_DSHARP5, keyboard.keys[KEY_SEMICOLON]);
				maintainNote(mix, sounds, AUDNOTE_E5, keyboard.keys[KEY_SLASH]);
			}
#else
			std::cout << "Sorry, this only works on Windows due Linux having no direct keyboard API.\n";
#endif
			return;
		}
		
		if (subcommand == "play")
		{
			if (argc > 1)
			{
				try
				{
					FileReader fr(argv[1]);
					MidiComposition c(fr);

					auto dev = audDevice::getDefault();
					std::cout << "Playing on " << dev.getName() << std::endl;
					auto pb = dev.open();
					audMixer mix;
					mix.setOutput(*pb);
					SharedPtr<audNoteEnvelope> sounds[AUDNOTE_SIZE];

					double t = 0.0;
					for (const auto& e : c.events)
					{
						if (e.t > t)
						{
							const double delta = e.t - t;
							std::this_thread::sleep_for(std::chrono::microseconds(static_cast<unsigned long long>(delta * 1'000'000.0f)));
							t = e.t;
						}

						switch (e.type)
						{
						case MIDI_NOTE_ON:
							maintainNote(mix, sounds, e.note, true);
							break;

						case MIDI_NOTE_OFF:
							maintainNote(mix, sounds, e.note, false);
							break;
						}
					}

					mix.stop_playback_when_done = true;
					pb->awaitCompletion();
				}
				catch (const std::exception& e)
				{
					std::cout << e.what() << std::endl;
				}
				return;
			}
		}
	}
	std::cout << "Syntax: soup midi [keyboard|play [file]]\n";
#else
	std::cout << "Sorry, audio is currently not supported on your platform.\n";
#endif
}
