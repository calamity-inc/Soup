#include "Layout.hpp"

#include "RasterFont.hpp"
#include "Rgb.hpp"
#include "RenderTarget.hpp"
#if SOUP_WINDOWS
#include "Window.hpp"
#endif

namespace soup
{
	using Element = Layout::Element;

	void Layout::draw(RenderTarget& rt)
	{
		size_t x = 0;
		size_t y = 0;
		for (const auto& block : blocks)
		{
			x += block.margin;
			y += block.margin;
			for (const auto& elm : block.elms)
			{
				const uint8_t elm_scale = (elm.font_size / font.baseline_glyph_height);
				rt.drawText(x, y, elm.text, font, Rgb::WHITE, elm_scale);
				x += ((font.measureWidth(elm.text) * elm_scale) + 1);
				x += ((font.get(' ').width * elm_scale) + 1);
			}
			x -= block.margin;
			y += (font.baseline_glyph_height * 2);
		}
	}

	Element* Layout::getElementAtPos(size_t ix, size_t iy)
	{
		size_t x = 0;
		size_t y = 0;
		for (auto& block : blocks)
		{
			x += block.margin;
			y += block.margin;
			size_t dy = y + (font.baseline_glyph_height * 2);
			for (auto& elm : block.elms)
			{
				const uint8_t elm_scale = (elm.font_size / font.baseline_glyph_height);
				size_t dx = x;
				dx += ((font.measureWidth(elm.text) * elm_scale) + 1);
				if (ix >= x
					&& iy >= y
					&& ix <= dx
					&& iy <= dy
					)
				{
					return &elm;
				}
				x = dx;
				x += ((font.get(' ').width * elm_scale) + 1);
			}
			x -= block.margin;
			y = dy;
		}
		return nullptr;
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
			if (w.getCustomData().get<Layout*>()->getElementAtPos(x, y) == nullptr)
			{
				return nullptr;
			}
			return [](Window w, int x, int y)
			{
				Layout& l = *w.getCustomData().get<Layout*>();
				Element& elm = *l.getElementAtPos(x, y);
				if (elm.on_click)
				{
					elm.on_click(elm, l);
					w.redraw();
				}
			};
		});
		w.setResizable(true);
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
