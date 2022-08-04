#include "lyoDocument.hpp"

#include "lyoFlatDocument.hpp"
#include "lyoTextElement.hpp"
#include "RenderTarget.hpp"
#include "Rgb.hpp"
#include "Window.hpp"

namespace soup
{
	lyoFlatDocument lyoDocument::flatten(int width, int height)
	{
		flat_x = 0;
		flat_y = 0;
		flat_width = width;
		flat_height = height;

		lyoFlatDocument flat;
		flattenElement(flat);
		return flat;
	}

	struct lyoWindowCapture
	{
		lyoDocument* doc;
		lyoFlatDocument flat;
	};

#if SOUP_WINDOWS
	Window lyoDocument::createWindow(const std::string& title)
	{
		auto w = Window::create(title, 200, 200);
		w.customData() = lyoWindowCapture{ this };
		w.setDrawFunc([](Window w, RenderTarget& rt)
		{
			lyoWindowCapture& cap = w.customData().get<lyoWindowCapture>();

			auto [width, height] = w.getSize();

			if (cap.doc->flat_width != width
				|| cap.doc->flat_height != height
				)
			{
				cap.flat = cap.doc->flatten(width, height);
			}

			rt.fill(Rgb::BLACK);
			cap.flat.draw(rt);
		});
		w.setMouseInformer([](Window w, unsigned int x, unsigned int y) -> Window::on_click_t
		{
			lyoWindowCapture& cap = w.customData().get<lyoWindowCapture>();
			auto elm = cap.flat.getElementAtPos(x, y);
			if (!elm || !elm->on_click)
			{
				return nullptr;
			}
			return [](Window w, unsigned int x, unsigned int y)
			{
				lyoWindowCapture& cap = w.customData().get<lyoWindowCapture>();
				auto elm = cap.flat.getElementAtPos(x, y);
				if (elm && elm->on_click != nullptr)
				{
					elm->on_click(*elm, *cap.doc);
					if (!cap.doc->isValid())
					{
						w.redraw();
					}
				}
			};
		});
		w.setResizable(true);
		return w;
	}
#endif
}
