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

using namespace soup;

static bool running = true;
#if SOUP_WINDOWS
static const char* kbd_name = "Digital Keyboard";
#else
static const char* kbd_name = "No Analogue Keyboard Detected";
#endif
static Window w;
static visKeyboard viskbd;

void cli_keyboard()
{
	Thread t([]
	{
#if SOUP_WINDOWS
		DigitalKeyboard digital_kbd;
#endif
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

					viskbd.has_ctx_key = false;
#if SOUP_WINDOWS
					kbd_name = "Digital Keyboard";
#else
					kbd_name = "No Analogue Keyboard Detected";
					w.redraw();
#endif
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
#if SOUP_WINDOWS
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
#endif

					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
				else
				{
					std::cout << "Found " << analogue_kbds.at(0).name << ", switching to analogue mode.\n";

					kbd_name = analogue_kbds.at(0).name;
					viskbd.has_ctx_key = analogue_kbds.at(0).has_ctx_key;
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
#if SOUP_WINDOWS
		rt.fill(Rgb::GREEN);
#else
		rt.fill(Rgb::GREY);
#endif
		rt.drawText(9, 9, kbd_name, RasterFont::simple8(), Rgb::WHITE, 3);
		viskbd.draw(rt, 0, (3 * 8) + 18, 5);
	});
#if SOUP_WINDOWS
	w.setInvisibleColour(Rgb::GREEN);
	w.setTopmost(true);
#endif
	w.runMessageLoop();

	running = false;
#if SOUP_WINDOWS
	CancelSynchronousIo(t.handle);
#endif
	t.awaitCompletion();
}
