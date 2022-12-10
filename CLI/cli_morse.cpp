#include "cli.hpp"

#include <cstring>
#include <iostream>

#include <audDevice.hpp>
#include <audMixer.hpp>
#include <audPlayback.hpp>
#include <audSound.hpp>
#include <morse.hpp>
#include <RasterFont.hpp>
#include <RenderTarget.hpp>
#include <Rgb.hpp>
#include <string.hpp>
#include <Window.hpp>

using namespace soup;

int cli_morse_key(bool silent)
{
#if SOUP_WINDOWS
	static MorseKey mk;

	UniquePtr<audPlayback> pb;
	audMixer mix;
	if (!silent)
	{
		auto dev = audDevice::getDefault();
		std::cout << "Playing on " << dev.getName() << "\n";
		pb = dev.open();
		mix.setOutput(*pb);
		mix.playSound(&mk);
	}

	auto w = Window::create("Soup Morse Key", 500, 100);
	w.setResizable(true);
	w.setDrawFunc([](Window w, RenderTarget& rt)
	{
		rt.fill(Rgb::BLACK);
		auto pattern = mk.seq.toPattern();
		rt.drawText(5, 50, Morse::decode(pattern), RasterFont::simple5(), Rgb::WHITE, 5);
		string::replaceAll(pattern, "-", "_");
		rt.drawText(5, 5, pattern, RasterFont::simple5(), Rgb::WHITE, 5);
	});
	w.setKeyCallback([](Window w, char32_t c, bool down, bool repeat)
	{
		if (!repeat)
		{
			if (mk.isDown() != down)
			{
				if (down)
				{
					mk.down();
				}
				else
				{
					mk.up();
				}
				w.redraw();
			}
		}
	});
	return w.runMessageLoop();
#else
	std::cout << "Sorry, this only works on Windows (for now).\n";
	return 0;
#endif
}

void cli_morse_encode(const char* arg, bool silent)
{
	auto pattern = Morse::encode(arg);
	std::cout << pattern << "\n";
	auto seq = Morse::patternToSequence(pattern);
	seq.alterndur.pop_back(); // stop instantly
	/*for (const auto& dur : seq.alterndur)
	{
		std::cout << dur << " ";
	}
	std::cout << "\n";*/
#if SOUP_WINDOWS
	if (!silent)
	{
		auto dev = audDevice::getDefault();
		std::cout << "Playing on " << dev.getName() << "\n";
		auto pb = dev.open();
		audMixer mix;
		mix.stop_playback_when_done = true;
		mix.setOutput(*pb);
		mix.playSound(&seq);
		pb->awaitCompletion();
	}
#endif
}

int cli_morse(int argc, const char** argv)
{
	if (argc > 0)
	{
		std::string subcommand = argv[0];
		string::lower(subcommand);

		if (subcommand == "key")
		{
			return cli_morse_key(argc > 1 && strcmp(argv[1], "--silent") == 0);
		}

		if (subcommand == "encode" && argc > 1)
		{
			cli_morse_encode(argv[1], (argc > 2 && strcmp(argv[2], "--silent") == 0));
			return 0;
		}
	}

	std::cout << "Syntax: soup morse [key|encode [text]] <--silent>\n";
	return 0;
}
