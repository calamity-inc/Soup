#include "editor.hpp"

#include <fstream>

#include "console.hpp"
#include "control_input.hpp"
#include "editor_text.hpp"
#include "unicode.hpp"

namespace soup
{
	static editor* inst;

	editor::editor()
		: conui_app()
	{
		inst = this;

		children.emplace_back(make_unique<editor_text>(this, 0, 1, 0, 0));

		console.char_handler.set([](char32_t c, const capture&)
		{
			if constexpr (false)
			{
				std::cout << (int)c << ".";
			}
			else
			{
				auto& edit = *inst;
				auto& text = edit.getTextChild();
				text.file.contents.at(text.file.y).insert(text.file.contents.at(text.file.y).cbegin() + (text.file.x++), c);
				edit.draw();
			}
		});
		console.control_handler.set([](control_input c, const capture&)
		{
			auto& edit = *inst;
			auto& text = edit.getTextChild();
			switch (c)
			{
			case UP:
				if (text.file.y != 0)
				{
					--text.file.y;
					edit.updateCursor();
				}
				break;

			case DOWN:
				if (text.file.y + 1 < text.file.contents.size())
				{
					++text.file.y;
					text.file.x = std::min(text.file.x, text.file.getLineWidth());
					edit.updateCursor();
				}
				break;

			case LEFT:
				if (text.file.x == 0)
				{
					if (text.file.y != 0)
					{
						--text.file.y;
						text.file.x = text.file.getLineWidth();
						edit.updateCursor();
					}
				}
				else
				{
					--text.file.x;
					edit.updateCursor();
				}
				break;

			case RIGHT:
				if (text.file.x + 1 <= text.file.getLineWidth())
				{
					++text.file.x;
				}
				else if (text.file.y + 1 < text.file.contents.size())
				{
					text.file.x = 0;
					++text.file.y;
				}
				edit.updateCursor();
				break;

			case NEW_LINE:
				text.file.contents.insert(text.file.contents.begin() + text.file.y + 1, text.file.contents.at(text.file.y).substr(text.file.x));
				text.file.contents.at(text.file.y).erase(text.file.x);
				text.file.x = 0;
				++text.file.y;
				edit.draw();
				break;

			case BACKSPACE:
				if (text.file.x == 0)
				{
					if (text.file.y != 0)
					{
						text.file.x = text.file.contents.at(text.file.y - 1).size();
						text.file.contents.at(text.file.y - 1).append(text.file.contents.at(text.file.y));
						text.file.contents.erase(text.file.contents.cbegin() + (text.file.y--));
						edit.draw();
					}
				}
				else
				{
					text.file.contents.at(text.file.y).erase(text.file.contents.at(text.file.y).cbegin() + (--text.file.x));
					edit.draw();
				}
				break;
			}
		});
		console.overrideCtrlC([]
		{
			auto& edit = *inst;
			for (auto i = edit.tabsBegin(); i != edit.tabsEnd(); ++i)
			{
				auto& tab = *reinterpret_cast<editor_tab*>(i->get());
				if (tab.isActive(edit))
				{
					tab.reclaimFile(edit);
				}
				std::ofstream of(tab.getName());
				for (const auto& line : tab.file.contents)
				{
					of << unicode::utf32_to_utf8(line) << std::endl;
				}
			}
			console.cleanup();
			exit(0);
		});
	}

	void editor::run() const
	{
		console.enableSizeTracking([](unsigned int width, unsigned int height, const capture& cap)
		{
			auto& edit = *cap.get<editor*>();
			edit.width = width;
			edit.height = height;
			edit.draw();
		}, this);
		console << "Measuring your terminal. Hold still.";
		console.run();
	}

	void editor::draw() const
	{
		console.setBackgroundColour(0, 0, 0);
		conui_app::draw();
		updateCursor();
		console.setBackgroundColour(0, 0, 0);
	}

	void editor::updateCursor() const
	{
		auto& text = getTextChild();
		console.setCursorPos(text.x + text.file.x, text.y + text.file.y);
	}

	std::vector<unique_ptr<conui_base>>::iterator editor::tabsBegin()
	{
		return children.begin() + 1;
	}

	std::vector<unique_ptr<conui_base>>::iterator editor::tabsEnd()
	{
		return children.end();
	}

	editor_tab& editor::addTab(std::string&& name, std::u32string&& text)
	{
		unsigned int x = 0;
		for (auto i = tabsBegin(); i != tabsEnd(); ++i)
		{
			x += (*i)->width;
		}
		return *reinterpret_cast<editor_tab*>(children.emplace_back(soup::make_unique<editor_tab>(this, x, 0, std::move(name), std::move(text))).get());
	}

	editor_text& editor::getTextChild() const noexcept
	{
		return *reinterpret_cast<editor_text*>(children.at(0).get());
	}
}
