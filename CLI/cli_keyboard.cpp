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
#if true
		for (const auto& kbd : AnalogueKeyboard::getAll())
		{
			kbd_name = kbd.name;
			w.redraw();
			while (running)
			{
				auto keys = kbd.getActiveKeys();
				viskbd.clear();
				for (const auto& key : keys)
				{
					//std::cout << std::hex << "Scancode " << (int)key.scancode << ", VK " << key.getVk() << ", VKT " << key.getVkTranslated() << "\n";
					if (key.hasUsbHidScancode())
					{
						viskbd.set(key.getUsbHidScancode(), key.value);
					}
					else if (key.scancode == 0x409)
					{
						viskbd.values[KEY_FN] = key.value;
					}
				}
				w.redraw();
			}
			return;
		}
#endif
		// Digital keyboard
		DigitalKeyboard kbd;
		while (running)
		{
			kbd.update();

			uint8_t dblbuf[NUM_KEYS];
			for (auto i = 0; i != NUM_KEYS; ++i)
			{
				dblbuf[i] = kbd.keys[i] ? 255 : 0;
			}

			if (memcmp(viskbd.values, dblbuf, sizeof(dblbuf)) != 0)
			{
				memcpy(viskbd.values, dblbuf, sizeof(dblbuf));
				w.redraw();
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
