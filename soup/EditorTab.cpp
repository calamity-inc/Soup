#include "EditorTab.hpp"

#include "console.hpp"
#include "Editor.hpp"
#include "EditorText.hpp"

NAMESPACE_SOUP
{
	void EditorTab::onClick(MouseButton b, unsigned int x, unsigned int y)
	{
		auto& edit = *static_cast<Editor*>(getApp());
		if (isActive(edit))
		{
			return;
		}
		edit.file->fg_colour = Rgb::WHITE;
		edit.file->bg_colour = unfocused_bg;
		edit.file->draw();
		edit.file->reclaimFile(edit);
		setActive(edit);
		/*draw();
		edit.getTextChild().draw();*/
		edit.draw();
	}

	bool EditorTab::isActive(Editor& edit) const
	{
		return edit.file == this;
	}

	void EditorTab::setActive(Editor& edit)
	{
		edit.file = this;
		fg_colour = Rgb::BLACK;
		bg_colour = Rgb::WHITE;
		edit.getTextChild().file = std::move(file);
		console.setTitle(std::move(std::string(text).append(" - soup edit")));
	}

	void EditorTab::reclaimFile(Editor& edit)
	{
		file = std::move(edit.getTextChild().file);
	}

	std::string EditorTab::getName() const
	{
		return text.substr(1, text.length() - 2);
	}
}
