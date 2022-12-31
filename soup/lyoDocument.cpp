#include "lyoDocument.hpp"

#include "lyoFlatDocument.hpp"
#include "lyoTextElement.hpp"
#include "RenderTarget.hpp"
#include "Rgb.hpp"
#include "Window.hpp"
#include "xml.hpp"

namespace soup
{
	lyoDocument::lyoDocument()
		: lyoContainer(nullptr)
	{
		tag_name = "body";

		lyoStylesheet uas;
		uas.name = "user-agent stylesheet";
		{
			lyoRule rule;
			rule.selector = "body";
			rule.style.setMargin(8);
			uas.rules.emplace_back(std::move(rule));
		}
		{
			lyoRule rule;
			rule.selector = "span";
			rule.style.display_inline = true;
			rule.style.font_size = 16;
			uas.rules.emplace_back(std::move(rule));
		}
		{
			lyoRule rule;
			rule.selector = "p";
			rule.style.display_inline = false;
			rule.style.font_size = 16;
			uas.rules.emplace_back(std::move(rule));
		}
		stylesheets.emplace_back(std::move(uas));
	}

	static void loadMarkup(lyoContainer* div, const XmlTag& tag)
	{
		for (const auto& node : tag.children)
		{
			if (node->is_text)
			{
				auto txt = div->addText(static_cast<const XmlText*>(node.get())->contents);
				txt->tag_name = tag.name;
			}
			else
			{
				auto inner_div = soup::make_unique<lyoContainer>(div);
				loadMarkup(inner_div.get(), *static_cast<const XmlTag*>(node.get()));
				div->children.emplace_back(std::move(inner_div));
			}
		}
	}

	UniquePtr<lyoDocument> lyoDocument::fromMarkup(const std::string& markup)
	{
		auto root = xml::parse(markup);
		return fromMarkup(*root);
	}

	UniquePtr<lyoDocument> lyoDocument::fromMarkup(const XmlTag& root)
	{
		auto doc = soup::make_unique<lyoDocument>();
		loadMarkup(doc.get(), root);
		doc->propagateStyle();
		return doc;
	}

	void lyoDocument::propagateStyle()
	{
		for (const auto& stylesheet : stylesheets)
		{
			for (const auto& rule : stylesheet.rules)
			{
				for (const auto& elm : querySelectorAll(rule.selector))
				{
					elm->style.overrideWith(rule.style);
				}
			}
		}
	}

	lyoFlatDocument lyoDocument::flatten(int width, int height)
	{
		flat_x = 0;
		flat_y = 0;
		flat_width = width;
		flat_height = height;

		lyoFlatDocument flat;
		populateFlatDocument(flat);
		updateFlatSize();
		updateFlatPos();
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
