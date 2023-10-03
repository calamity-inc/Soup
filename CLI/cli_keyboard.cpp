#include "cli.hpp"

#include <iostream>
#include <thread>

#include <AnalogueKeyboard.hpp>
#include <DigitalKeyboard.hpp>
#include <RasterFont.hpp>
#include <RenderTarget.hpp>
#include <Rgb.hpp>
#include <Thread.hpp>
#include <visKeyboard.hpp>
#include <Window.hpp>

#define FORCE_DIGITAL false

#if SOUP_WINDOWS
using namespace soup;

static bool running = true;
static const char* kbd_name = "Digital Keyboard";
static Window w;
static visKeyboard viskbd;
#endif

void cli_keyboard()
{
#if SOUP_WINDOWS
	Thread t([]
	{
		DigitalKeyboard digital_kbd;
		AnalogueKeyboard* analogue_kbd = nullptr;

		while (running)
		{
			if (analogue_kbd)
			{
				SOUP_IF_UNLIKELY (analogue_kbd->disconnected)
				{
					std::cout << analogue_kbd->name << " disconnected, switching back to digital mode.\n";

					delete analogue_kbd;
					analogue_kbd = nullptr;

					kbd_name = "Digital Keyboard";
				}
				else
				{
					auto keys = analogue_kbd->getActiveKeys();
					viskbd.clear();
					for (const auto& key : keys)
					{
						//std::cout << std::hex << "Scancode " << (int)key.scancode << ", SK " << (int)key.getSoupKey() << ", VK " << key.getVk() << ", VKT " << key.getVkTranslated() << "\n";
						if (auto sk = key.getSoupKey(); sk != KEY_NONE)
						{
							viskbd.values[sk] = key.value;
						}
					}
					w.redraw();
				}
			}
			else
			{
				std::vector<AnalogueKeyboard> analogue_kbds{};
#if !FORCE_DIGITAL
				analogue_kbds = AnalogueKeyboard::getAll();
#endif
				if (analogue_kbds.empty())
				{
					digital_kbd.update();

					uint8_t dblbuf[NUM_KEYS];
					for (auto i = 0; i != NUM_KEYS; ++i)
					{
						dblbuf[i] = digital_kbd.keys[i] ? 255 : 0;
					}

					if (memcmp(viskbd.values, dblbuf, sizeof(dblbuf)) != 0)
					{
						memcpy(viskbd.values, dblbuf, sizeof(dblbuf));
						w.redraw();
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
				else
				{
					std::cout << "Found " << analogue_kbds.at(0).name << ", switching to analogue mode.\n";

					kbd_name = analogue_kbds.at(0).name;
					w.redraw();

					analogue_kbd = new AnalogueKeyboard(std::move(analogue_kbds.at(0)));
				}
			}
		}

		if (analogue_kbd)
		{
			delete analogue_kbd;
		}
	});

	w = Window::create("Soup", 195 * 5, (3 * 8) + 18 + 50 * 5);
	w.setDrawFunc([](Window, RenderTarget& rt)
	{
		rt.fill(Rgb::GREEN);
		rt.drawText(9, 9, kbd_name, RasterFont::simple8(), Rgb::WHITE, 3);
		viskbd.draw(rt, 0, (3 * 8) + 18, 5);
	});
	w.setInvisibleColour(Rgb::GREEN);
	w.setTopmost(true);
	w.runMessageLoop();

	running = false;
	CancelSynchronousIo(t.handle);
	t.awaitCompletion();
#else
	std::cout << "Sorry, this only works on Windows (for now).\n";
#endif
}
