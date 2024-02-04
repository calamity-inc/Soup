#include "main.hpp"

#include <iostream>
#include <thread>

#include <RenderTarget.hpp>
#include <Rgb.hpp>
#include <Thread.hpp>
#include <Window.hpp>

using namespace soup;

static bool running = true;
static Window w;

void cli_mouse()
{
#if SOUP_WINDOWS
	Thread t([](Capture&&)
	{
		while (running)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			w.redraw();
		}
	});

	static constexpr auto scale = 5;
	w = Window::create("Soup Mouse Visualisation", 25 * scale, 40 * scale);
	w.setDrawFunc([](Window, RenderTarget& rt)
	{
		rt.fill(Rgb::GREEN);
		rt.drawRect(0, 0, 10 * scale, 20 * scale, (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? Rgb::MAGENTA : Rgb::BLACK);
		rt.drawRect(10 * scale, 0, 5 * scale, 20 * scale, (GetAsyncKeyState(VK_MBUTTON) & 0x8000) ? Rgb::MAGENTA : Rgb::BLACK);
		rt.drawRect(15 * scale, 0, 10 * scale, 20 * scale, (GetAsyncKeyState(VK_RBUTTON) & 0x8000) ? Rgb::MAGENTA : Rgb::BLACK);
		rt.drawRect(0, 20 * scale, 25 * scale, 20 * scale, Rgb::BLACK);
	});
	w.setInvisibleColour(Rgb::GREEN);
	w.setTopmost(true);
	w.runMessageLoop();

	running = false;
	t.awaitCompletion();
#else
	std::cout << "Sorry, only available on Windows!\n";
#endif
}
