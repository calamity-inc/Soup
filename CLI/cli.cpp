#include "cli.hpp"

#include <iostream>

#include <canvas.hpp>
#include <chess_cli.hpp>
#include <console.hpp>
#include <editor.hpp>
#include <qr_code.hpp>
#include <string.hpp>
#include <unicode.hpp>

using namespace soup;

int main(int argc, const char** argv)
{
	if (argc > 1)
	{
		std::string subcommand = argv[1];
		string::lower(subcommand);

		if (subcommand == "qr")
		{
			if (argc != 3)
			{
				std::cout << "Syntax: soup qr [contents]" << std::endl;
				return 0;
			}
			auto qrcode = qr_code::encodeText(argv[2]);
			console.init(false);
			console << qrcode.toCanvas(4, true).toStringDownsampledDoublewidth(true);
			console.resetColour();
			return 0;
		}

		if (subcommand == "edit")
		{
			if (argc <= 2)
			{
				std::cout << "Syntax: soup edit [files...]" << std::endl;
				return 0;
			}
			editor edit{};
			for (int i = 2; i != argc; ++i)
			{
				auto& tab = edit.addTab(argv[i], unicode::utf8_to_utf32(string::fromFile(argv[i])));
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
			chess_cli cc{};
			if (argc > 2)
			{
				cc.board.loadFen(argv[2]);
			}
			cc.run();
			return 0;
		}

		if (subcommand == "3d")
		{
			cli_3d();
			return 0;
		}
	}

	std::cout << R"EOC(Syntax: soup [tool]

Available tools:
- qr [contents]
- edit [files ...]
- chess <FEN>
- 3d

Legend: [Required] <Optional>)EOC" << std::endl;
	return 0;
}
