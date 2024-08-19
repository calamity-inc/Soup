#include "lyoDocument.hpp"

#include "joaat.hpp"
#include "lyoFlatDocument.hpp"
#include "lyoInputElement.hpp"
#include "lyoTextElement.hpp"
#include "RenderTarget.hpp"
#include "Rgb.hpp"
#include "Window.hpp"
#include "xml.hpp"

#if LYO_DEBUG_STYLE
#include "format.hpp"
#include "log.hpp"
#endif

NAMESPACE_SOUP
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
			rule.style.display_block = true;
			rule.style.setMargin(8);
			rule.style.font_size = 16;
			uas.rules.emplace_back(std::move(rule));
		}
		{
			lyoRule rule;
			rule.selector = "p";
			rule.style.display_block = true;
			uas.rules.emplace_back(std::move(rule));
		}
		stylesheets.emplace_back(std::move(uas));
	}

	static void loadMarkup(lyoDocument& doc, lyoContainer* div, const XmlTag& tag)
	{
		for (const auto& node : tag.children)
		{
			if (node->is_text)
			{
				div->addText(static_cast<const XmlText*>(node.get())->contents);
			}
			else
			{
				switch (joaat::hash(static_cast<const XmlTag*>(node.get())->name))
				{
				case joaat::compileTimeHash("input"):
					{
						auto input = soup::make_unique<lyoInputElement>(div);
						if (doc.focus == nullptr)
						{
							doc.focus = input.get();
						}
						div->children.emplace_back(std::move(input));
					}
					break;

				case joaat::compileTimeHash("style"):
					for (const auto& child : static_cast<const XmlTag*>(node.get())->children)
					{
						if (child->isText())
						{
							lyoStylesheet& stylesheet = doc.stylesheets.emplace_back();
							stylesheet.name = "<style> tag";
							stylesheet.addRulesFromCss(static_cast<const XmlText*>(child.get())->contents);
						}
					}
					break;

				default:
					{
						auto inner_div = soup::make_unique<lyoContainer>(div);
						inner_div->tag_name = static_cast<const XmlTag*>(node.get())->name;
						loadMarkup(doc, inner_div.get(), *static_cast<const XmlTag*>(node.get()));
						div->children.emplace_back(std::move(inner_div));
					}
					break;
				}
			}
		}
	}

	UniquePtr<lyoDocument> lyoDocument::fromMarkup(const std::string& markup)
	{
		auto root = xml::parseAndDiscardMetadata(markup, xml::MODE_HTML);
		return fromMarkup(*root);
	}

	UniquePtr<lyoDocument> lyoDocument::fromMarkup(const XmlTag& root)
	{
		auto doc = soup::make_unique<lyoDocument>();
		lyoContainer* import_root = doc.get();

		// The tag_name of a lyoDocument should be "body", so if that does not match with the xml, the import root needs to be child of the document, not the document itself.
		if (root.name != "body")
		{
			import_root = static_cast<lyoContainer*>(doc->children.emplace_back(soup::make_unique<lyoContainer>(doc.get())).get());
			import_root->tag_name = root.name;
		}

		loadMarkup(*doc, import_root, root);
		doc->propagateStyle();
		return doc;
	}

	void lyoDocument::propagateStyle()
	{
		for (const auto& stylesheet : stylesheets)
		{
			for (const auto& rule : stylesheet.rules)
			{
				auto elms = querySelectorAll(rule.selector);
#if LYO_DEBUG_STYLE
				logWriteLine(format("{} matched {} elements", rule.selector, elms.size()));
#endif
				for (const auto& elm : elms)
				{
					elm->style.overrideWith(rule.style);
				}
			}
		}

		propagateStyleToChildren();
	}

	lyoFlatDocument lyoDocument::flatten(int width, int height)
	{
		lyoFlatDocument flat;
		populateFlatDocument(flat);

		flat_width = width;
		flat_height = height;

		unsigned int x = 0;
		unsigned int y = 0;
		unsigned int wrap_y = 0;
		updateFlatValues(x, y, wrap_y);

		return flat;
	}

	struct lyoWindowCapture
	{
		lyoDocument* doc;
		lyoFlatDocument flat;
	};

	Window lyoDocument::createWindow(const std::string& title)
	{
		auto w = Window::create(title, 200, 200);
		w.customData() = lyoWindowCapture{ this };
		w.setDrawFunc([](Window w, RenderTarget& rt)
		{
			lyoWindowCapture& cap = w.customData().get<lyoWindowCapture>();

			// Handle resize here, I guess.
			if (cap.doc->flat_width != rt.width
				|| cap.doc->flat_height != rt.height
				)
			{
				cap.flat = cap.doc->flatten(rt.width, rt.height);
			}

			rt.fill(Rgb::BLACK);
			cap.flat.draw(rt);
		});
#if SOUP_WINDOWS
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
				if (elm && elm->on_click)
				{
					elm->on_click(*elm, *cap.doc);
					if (!cap.doc->isValid())
					{
						w.redraw();
					}
				}
			};
		});
		w.setCharCallback([](Window w, char32_t c)
		{
			lyoWindowCapture& cap = w.customData().get<lyoWindowCapture>();
			auto elm = cap.doc->focus;
			if (elm && elm->on_char)
			{
				elm->on_char(c, *elm, *cap.doc);
				if (!cap.doc->isValid())
				{
					w.redraw();
				}
			}
		});
		w.setResizable(true);
#endif
		return w;
	}
}
