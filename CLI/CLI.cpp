#include <iostream>

#include <chess_cli.hpp>
#include <editor.hpp>
#include <string.hpp>
#include <unicode.hpp>

int main(int argc, const char** argv)
{
	if (argc > 1)
	{
		std::string subcommand = argv[1];
		soup::string::toLower(subcommand);

		if (subcommand == "edit")
		{
			if (argc <= 2)
			{
				std::cout << "Syntax: soup edit [files...]" << std::endl;
				return 0;
			}
			soup::editor edit{};
			for (int i = 2; i != argc; ++i)
			{
				auto& tab = edit.addTab(argv[i], soup::unicode::utf8_to_utf32(soup::string::fromFile(argv[i])));
				if (i == 2)
				{
					tab.setActive(edit);
				}
			}
			edit.run();
			return 0;
		}

		if (subcommand == "chess")
		{
			soup::chess_cli cc{};
			if (argc > 2)
			{
				cc.board.loadFen(argv[2]);
			}
			cc.run();
			return 0;
		}
	}

	std::cout << R"EOC(Syntax: soup [tool]

Available tools:
- edit [files ...]
- chess <FEN>

Legend: [Required] <Optional>)EOC" << std::endl;
	return 0;
}
