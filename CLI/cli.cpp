#include "cli.hpp"

#include <iostream>

#include <Canvas.hpp>
#include <ChessCli.hpp>
#include <console.hpp>
#include <Editor.hpp>
#include <QrCode.hpp>
#include <string.hpp>
#include <unicode.hpp>

using namespace soup;

int main(int argc, const char** argv)
{
	if (argc > 1)
	{
		std::string subcommand = argv[1];
		string::lower(subcommand);

		if (subcommand == "3d")
		{
			cli_3d();
			return 0;
		}

		if (subcommand == "chess")
		{
			ChessCli cc{};
			if (argc > 2)
			{
				cc.board.loadFen(argv[2]);
			}
			cc.run();
			return 0;
		}

		if (subcommand == "dvd")
		{
			cli_dvd();
			return 0;
		}

		if (subcommand == "edit")
		{
			if (argc <= 2)
			{
				std::cout << "Syntax: soup edit [files...]" << std::endl;
				return 0;
			}
			Editor edit{};
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

		if (subcommand == "maze")
		{
			cli_maze();
			return 0;
		}

		if (subcommand == "qr")
		{
			if (argc != 3)
			{
				std::cout << "Syntax: soup qr [contents]" << std::endl;
				return 0;
			}
			auto qrcode = QrCode::encodeText(argv[2]);
			console.init(false);
			console << qrcode.toCanvas(4, true).toStringDownsampledDoublewidth(true);
			console.resetColour();
			return 0;
		}

		if (subcommand == "repl")
		{
			cli_repl();
			return 0;
		}

		if (subcommand == "snake")
		{
			cli_snake();
			return 0;
		}

		if (subcommand == "test")
		{
			cli_test();
			return 0;
		}

		if (subcommand == "websrv")
		{
			if (argc <= 2)
			{
				std::cout << "Syntax: soup websrv [dir]" << std::endl;
				return 0;
			}
			return cli_websrv(argv[2]);
		}
	}

	std::cout << R"EOC(Syntax: soup [tool]

Available tools:
- 3d
- chess <FEN>
- dvd
- edit [files...]
- maze
- qr [contents]
- repl
- snake
- test
- websrv [dir]

Legend: [Required] <Optional>)EOC" << std::endl;
	return 0;
}
