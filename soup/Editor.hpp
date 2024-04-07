#pragma once

#include "ConuiApp.hpp"

#include "EditorTab.hpp"

NAMESPACE_SOUP
{
	struct Editor : public ConuiApp
	{
		unsigned int width;
		unsigned int height;

		EditorTab* file;

		Editor();

		void run() const;

		void draw() const final;
		void updateCursor() const;

		[[nodiscard]] std::vector<UniquePtr<ConuiBase>>::iterator tabsBegin();
		[[nodiscard]] std::vector<UniquePtr<ConuiBase>>::iterator tabsEnd();
		EditorTab& addTab(std::string&& name, std::u32string&& text);

		[[nodiscard]] EditorText& getTextChild() const noexcept;
	};
}
