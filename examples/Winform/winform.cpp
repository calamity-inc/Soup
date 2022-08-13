#include <soup/RasterFont.hpp>
#include <soup/Rgb.hpp>
#include <soup/RenderTarget.hpp>
#include <soup/main.hpp>
#include <soup/Window.hpp>

int entry(std::vector<std::string>&& args, bool console)
{
	auto w = soup::Window::create("C++ Winform", 250, 250);
	w.setDrawFunc([](soup::Window, soup::RenderTarget& rt)
	{
		auto font8 = soup::RasterFont::simple8();

		rt.fill(soup::Rgb::BLACK);
		rt.drawText(1, 1, "The quick brown fox jumps over the lazy dog.", font8, soup::Rgb::WHITE);
	});
	return w.runMessageLoop();
}

SOUP_MAIN_GUI(&entry);
