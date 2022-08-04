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

		void ensureFlat(const RenderTarget& rt)
		{
			if (doc->flat_width != rt.width
				|| doc->flat_height != rt.height
				)
			{
				flat = doc->flatten(rt.width, rt.height);
			}
		}
	};

#if SOUP_WINDOWS
	Window lyoDocument::createWindow(const std::string& title)
	{
		auto w = Window::create(title, 200, 200);
		w.customData() = lyoWindowCapture{ this };
		w.setDrawFunc([](Window w, RenderTarget& rt)
		{
			lyoWindowCapture& cap = w.customData().get<lyoWindowCapture>();
			cap.ensureFlat(rt);
			rt.fill(Rgb::BLACK);
			cap.flat.draw(rt);
		});
		w.setMouseInformer([](Window w, int x, int y) -> Window::on_click_t
		{
			lyoWindowCapture& cap = w.customData().get<lyoWindowCapture>();
			if (!cap.flat.isInitialised()
				|| cap.flat.getElementAtPos(x, y).on_click == nullptr
				)
			{
				return nullptr;
			}
			return [](Window w, int x, int y)
			{
				lyoWindowCapture& cap = w.customData().get<lyoWindowCapture>();
				lyoElement& elm = cap.flat.getElementAtPos(x, y);
				if (elm.on_click != nullptr)
				{
					elm.on_click(elm, *cap.doc);
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
