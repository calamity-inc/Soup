#include "cli.hpp"

#include <iostream>

#include <Canvas.hpp>
#include <ChessCli.hpp>
#include <CompiledExecutable.hpp>
#include <country_names.hpp>
#include <console.hpp>
#include <Editor.hpp>
#include <InquiryLang.hpp>
#include <netIntel.hpp>
#include <os.hpp>
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

		if (subcommand == "dig")
		{
			cli_dig(argc - 2, &argv[2]);
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

		if (subcommand == "geoip")
		{
			if (argc != 3)
			{
				std::cout << "Syntax: soup geoip [ip]" << std::endl;
				return 0;
			}
			IpAddr addr = argv[2];
			if (addr.isV4())
			{
				std::cout << "Initialising netIntel for IPv4..." << std::endl;
				netIntel::init(true, false);
			}
			else
			{
				std::cout << "Initialising netIntel for IPv6..." << std::endl;
				netIntel::init(false, true);
			}
			if (auto loc = netIntel::getLocationByIp(addr))
			{
				if (loc->city)
				{
					std::cout << "City: " << loc->city << std::endl;
				}
				if (loc->state)
				{
					std::cout << "State: " << loc->state << std::endl;
				}
				if (auto cn = getCountryName(loc->country_code.c_str()))
				{
					std::cout << "Country: " << cn << std::endl;
				}
				else
				{
					std::cout << "Country: " << loc->country_code.c_str() << std::endl;
				}
			}
			if (auto as = netIntel::getAsByIp(addr))
			{
				std::cout << "AS Number: " << as->number << "\n";
				std::cout << "AS Handle: " << as->handle << "\n";
				std::cout << "AS Name: " << as->name << "\n";
				std::cout << "AS is hosting provider? " << (as->isHosting() ? "Yes" : "No") << "\n";
			}
			return 0;
		}

		if (subcommand == "inquire")
		{
			console.init(false);
			std::cout << "Soup Inquiry Language REPL" << std::endl;
			while (true)
			{
				std::cout << "> ";
				std::string line;
				if (!std::getline(std::cin, line))
				{
					break;
				}
				try
				{
					std::cout << InquiryLang::formatResult(InquiryLang::execute(line));
				}
				catch (std::exception& e)
				{
					std::cout << "Error: " << e.what() << std::endl;
				}
			}
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

		if (subcommand == "script")
		{
			if (argc != 3)
			{
				std::cout << "Syntax: soup script [.cpp file]" << std::endl;
				return 0;
			}
			auto res = CompiledExecutable::fromCpp(argv[2]);
			std::cout << res.compiler_output;
			if (res.exe_file.exists())
			{
				std::cout << os::execute(res.exe_file);
			}
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
			if (argc != 3)
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
- dig [domain] <type=A> <@<doh:>[server]>
- dvd
- edit [files...]
- geoip [ip]
- inquire
- maze
- qr [contents]
- repl
- script [.cpp file]
- snake
- test
- websrv [dir]

Legend: [Required] <Optional>)EOC" << std::endl;
	return 0;
}
