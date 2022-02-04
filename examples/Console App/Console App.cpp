#include <console.hpp>

int main()
{
	soup::console.init();

	soup::console.setTitle("Souplime Text");

	soup::console.fillScreen(40, 41, 35);
	soup::console.setCursorPos(1, 1);

	soup::console.overrideCtrlC([]
	{
		soup::console.cleanup();
		soup::console << "Bye, bye!\n";
		exit(0);
	});

	soup::console.input_handler = [](char32_t ch)
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

	soup::console.onMouseClick([](soup::mouse_button b, int x, int y)
	{
		if (b == soup::LMB)
		{
			soup::console.setCursorPos(x, y);
		}
	});

	soup::console.run();
}
