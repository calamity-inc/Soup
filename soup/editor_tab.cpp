#include "editor_tab.hpp"

#include "console.hpp"
#include "editor.hpp"
#include "editor_text.hpp"

namespace soup
{
	void editor_tab::onClick(mouse_button b, unsigned int x, unsigned int y)
	{
		auto& edit = *reinterpret_cast<editor*>(getApp());
		if (isActive(edit))
		{
			return;
		}
		edit.file->fg_colour = rgb::WHITE;
		edit.file->bg_colour = unfocused_bg;
		edit.file->draw();
		edit.file->reclaimFile(edit);
		setActive(edit);
		/*draw();
		edit.getTextChild().draw();*/
		edit.draw();
	}

	bool editor_tab::isActive(editor& edit) const
	{
		return edit.file == this;
	}

	void editor_tab::setActive(editor& edit)
	{
		edit.file = this;
		fg_colour = rgb::BLACK;
		bg_colour = rgb::WHITE;
		edit.getTextChild().file = std::move(file);
		console.setTitle(std::move(std::string(text).append(" - soup edit")));
	}

	void editor_tab::reclaimFile(editor& edit)
	{
		file = std::move(edit.getTextChild().file);
	}

	std::string editor_tab::getName() const
	{
		return text.substr(1, text.length() - 2);
	}
}
