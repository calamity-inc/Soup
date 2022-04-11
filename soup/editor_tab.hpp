#pragma once

#include "conui_span.hpp"

#include "editor_file.hpp"

namespace soup
{
	struct editor_tab : public conui_span
	{
		editor_file file;

		static constexpr rgb unfocused_bg = { 30, 30, 30 };

		editor_tab(conui_div* parent, unsigned int x, unsigned int y, std::string&& name, std::u32string&& contents)
			: conui_span(parent, x, y, std::move(std::string(1, ' ').append(name).append(1, ' ')), rgb::WHITE, unfocused_bg), file{ { std::move(contents) } }
		{
			// close x = U+D7
			// unsaved dot = U+2022
		}

		void onClick(mouse_button b, unsigned int x, unsigned int y) final;

		[[nodiscard]] bool isActive(editor& edit) const;
		void setActive(editor& edit);
		void reclaimFile(editor& edit);

		[[nodiscard]] std::string getName() const;
	};
}
