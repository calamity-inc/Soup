#include <console.hpp>

static soup::console c{};

int main()
{
	c.setTitle("Souplime Text");

	c.fillScreen(40, 41, 35);
	c.setCursorPos(1, 1);

	c.overrideCtrlC([]
	{
		c.cleanup();
		c << "Bye, bye!\n";
		exit(0);
	});

	c.input_handler = [](char32_t ch)
	{
		if constexpr (true)
		{
			std::cout << soup::unicode::utf32_to_utf8(ch);
		}
		else
		{
			std::cout << (int)ch;
			std::cout << ".";
		}
	};

	c.onMouseClick([](soup::mouse_button b, int x, int y)
	{
		if (b == soup::LMB)
		{
			soup::console::setCursorPos(x, y);
		}
	});

	c.run();
}
