#include <soup/Canvas.hpp>
#include <soup/main.hpp>
#include <soup/QrCode.hpp>
#include <soup/Window.hpp>

HBRUSH brush_black = CreateSolidBrush(RGB(0, 0, 0));
HBRUSH brush_white = CreateSolidBrush(RGB(255, 255, 255));

int entry(std::vector<std::string>&& args, bool console)
{
	auto w = soup::Window::create("C++ Winform", 200, 200, "a.ico");
	w.setDrawFunc([](soup::Window, HDC hdc)
	{
		auto c = soup::QrCode::encodeText("Hello, world!").toCanvas(4, true);
		for (size_t y = 0; y != c.height; ++y)
		{
			for (size_t x = 0; x != c.width; ++x)
			{
				constexpr auto scale = 7;

				RECT r;
				r.left = x * scale;
				r.top = y * scale;
				r.right = (x + 1) * scale;
				r.bottom = (y + 1) * scale;
				FillRect(hdc, &r, c.get(x, y) == soup::Rgb::BLACK ? brush_black : brush_white);
			}
		}
	});
	w.setBorderless();
	w.setExitOnClose();
	w.show();
	return w.runMessageLoop();
}

SOUP_MAIN_GUI(&entry);
