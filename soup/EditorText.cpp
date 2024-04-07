#include "EditorText.hpp"

#include "console.hpp"
#include "unicode.hpp"

NAMESPACE_SOUP
{
	void EditorText::draw() const
	{
		console.setCursorPos(x, y);
		console.setBackgroundColour(0, 0, 0);
		console.setForegroundColour(255, 255, 255);
		for (const auto& line : file.contents)
		{
			console << unicode::utf32_to_utf8(line) << "\n";
		}
	}
}
