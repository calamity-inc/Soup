#include "lyoInputElement.hpp"

#include "lyoDocument.hpp"

NAMESPACE_SOUP
{
	lyoInputElement::lyoInputElement(lyoContainer* parent)
		: lyoTextElement(parent, {})
	{
		tag_name = "input";
		on_char = [](char32_t c, lyoElement& elm, lyoDocument& doc)
		{
			switch (c)
			{
			default:
				static_cast<lyoInputElement&>(elm).text.push_back(c);
				doc.invalidate();
				break;

			case 8: // Backspace
				if (!static_cast<lyoInputElement&>(elm).text.empty())
				{
					static_cast<lyoInputElement&>(elm).text.pop_back();
					doc.invalidate();
				}
				break;

			case 1: // Select All
			case 2: // Ctrl+B
			case 3: // Copy
			case 4: // Ctrl+D
			case 5: // Ctrl+E
			case 6: // Ctrl+F
			case 7: // Ctrl+G
			case 10: // Ctrl+J
			case 11: // Ctrl+K
			case 12: // Ctrl+L
			case 13: // Enter
			case 14: // Ctrl+N
			case 15: // Ctrl+O
			case 16: // Ctrl+P
			case 17: // Ctrl+Q
			case 18: // Ctrl+R
			case 19: // Ctrl+S
			case 20: // Ctrl+T
			case 21: // Ctrl+U
			case 22: // Paste
			case 23: // Ctrl+W
			case 24: // Cut
			case 25: // Redo
			case 26: // Undo
			case 27: // Escape
			case 127: // Ctrl+Backspace
				break;
			}
		};
	}
}
