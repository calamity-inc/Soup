#include "Layout.hpp"

#include "RasterFont.hpp"
#include "Rgb.hpp"
#include "RenderTarget.hpp"
#if SOUP_WINDOWS
#include "Window.hpp"
#endif

namespace soup
{
	void Layout::draw(RenderTarget& rt)
	{
		auto font = RasterFont::simple8();

		size_t x = 0;
		size_t y = 0;
		for (const auto& block : blocks)
		{
			x += block.margin;
			y += block.margin;
			for (const auto& elm : block.elms)
			{
				rt.drawText(x, y, elm.text, font, Rgb::WHITE, elm.font_size / font.baseline_glyph_height);
				x += (font.measureWidth(elm.text) + 1);
				x += (font.get(' ').width + 1);
			}
			x -= block.margin;
			y += (font.baseline_glyph_height * 2);
		}
	}

#if SOUP_WINDOWS
	Window Layout::createWindow(const std::string& title)
	{
		auto w = Window::create(title, 200, 200);
		w.getCustomData() = this;
		w.setDrawFunc([](Window w, RenderTarget& rt)
		{
			rt.fill(Rgb::BLACK);
			w.getCustomData().get<Layout*>()->draw(rt);
		});
		w.setMouseInformer([](Window w, int x, int y) -> Window::on_click_t
		{
			/*if (x >= 20
				&& y >= 20
				&& x <= 40
				&& y <= 40
				)
			{
				return [](Window w, int x, int y)
				{
					toggle = !toggle;
					w.redraw();
				};
			}*/
			return nullptr;
		});
		w.setExitOnClose();
		w.setResizable(true);
		w.show();
		/*std::thread t([]
		{
			while (true)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
				w.redraw();
			}
		});
		t.detach();*/
		return w;
	}
#endif
}
