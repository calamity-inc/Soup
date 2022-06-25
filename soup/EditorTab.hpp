#pragma once

#include "ConuiSpan.hpp"

#include "EditorFile.hpp"

namespace soup
{
	struct EditorTab : public ConuiSpan
	{
		EditorFile file;

		static constexpr Rgb unfocused_bg{ 30, 30, 30 };

		EditorTab(ConuiDiv* parent, unsigned int x, unsigned int y, std::string&& name, std::u32string&& contents)
			: ConuiSpan(parent, x, y, std::move(std::string(1, ' ').append(name).append(1, ' ')), Rgb::WHITE, unfocused_bg), file{ { std::move(contents) } }
		{
			// close x = U+D7
			// unsaved dot = U+2022
		}

		void onClick(MouseButton b, unsigned int x, unsigned int y) final;

		[[nodiscard]] bool isActive(Editor& edit) const;
		void setActive(Editor& edit);
		void reclaimFile(Editor& edit);

		[[nodiscard]] std::string getName() const;
	};
}
