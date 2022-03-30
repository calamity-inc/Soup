#pragma once

#include "conui_app.hpp"

#include "editor_tab.hpp"

namespace soup
{
	struct editor : public conui_app
	{
		unsigned int width;
		unsigned int height;

		editor_tab* file;

		editor();

		void run() const;

		void draw() const final;
		void updateCursor() const;

		[[nodiscard]] std::vector<unique_ptr<conui_base>>::iterator tabsBegin();
		[[nodiscard]] std::vector<unique_ptr<conui_base>>::iterator tabsEnd();
		editor_tab& addTab(std::string&& name, std::u32string&& text);

		[[nodiscard]] editor_text& getTextChild() const noexcept;
	};
}
