#include "cli.hpp"

#include <iostream>

#include <audDevice.hpp>
#include <audPlayback.hpp>
#include <Canvas.hpp>
#include <Chatbot.hpp>
#include <ChessCli.hpp>
#include <CompiledExecutable.hpp>
#include <country_names.hpp>
#include <console.hpp>
#include <dhcp.hpp>
#include <Editor.hpp>
#include <FileReader.hpp>
#include <HttpRequest.hpp>
#include <hwHid.hpp>
#include <InquiryLang.hpp>
#include <netIntel.hpp>
#include <os.hpp>
#include <QrCode.hpp>
#include <riff.hpp>
#include <string.hpp>
#include <unicode.hpp>
#include <Uri.hpp>
#include <WavFmtChunk.hpp>

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

		if (subcommand == "chatbot")
		{
			while (true)
			{
				console.init(false);
				std::cout << "> ";
				std::string line;
				if (!std::getline(std::cin, line))
				{
					break;
				}
				try
				{
					auto res = Chatbot::process(line);
					std::cout << res.response << std::endl;
					if (res.type == CB_RES_IMAGE)
					{
						std::cout << res.extra.getCanvas().toStringDownsampledDoublewidthUtf8(true);
					}
				}
				catch (std::exception& e)
				{
					std::cout << "Error: " << e.what() << std::endl;
				}
			}
			return 0;
		}

		if (subcommand == "chatgpt")
		{
			if (argc > 2)
			{
				cli_chatgpt(argc - 2, &argv[2]);
			}
			else
			{
				std::cout << "Syntax: soup chatgpt [token] <model>" << std::endl;
			}
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

		if (subcommand == "datareflection")
		{
			cli_datareflection();
			return 0;
		}

		if (subcommand == "dhcp")
		{
			if (auto info = dhcp::requestInfo(); info.isValid())
			{
				std::cout << "Router \"" << info.server_name << "\" responded to DHCPInform and provided DNS server " << IpAddr(info.dns_server).toString() << "\n";
			}
			else
			{
				std::cout << "I screamed into the void, but the void did not respond. Possibly VPN network?\n";
			}
			return 0;
		}

		if (subcommand == "dig")
		{
			cli_dig(argc - 2, &argv[2]);
			return 0;
		}

		if (subcommand == "dnsserver")
		{
			if (argc > 2)
			{
				cli_dnsserver(argc - 2, &argv[2]);
			}
			else
			{
				std::cout << "Syntax: soup dnsserver [file] <bind-ip>" << std::endl;
			}
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
			netIntel intel;
			if (addr.isV4())
			{
				std::cout << "Initialising netIntel for IPv4..." << std::endl;
				intel.init(true, false);
			}
			else
			{
				std::cout << "Initialising netIntel for IPv6..." << std::endl;
				intel.init(false, true);
			}
			if (auto loc = intel.getLocationByIp(addr))
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
			if (auto as = intel.getAsByIp(addr))
			{
				std::cout << "AS Number: " << as->number << "\n";
				std::cout << "AS Handle: " << as->handle << "\n";
				std::cout << "AS Name: " << as->name << "\n";
				std::cout << "AS is hosting provider? " << (as->isHosting() ? "Yes" : "No") << "\n";
			}
			return 0;
		}

		if (subcommand == "hid")
		{
			std::cout << std::hex;
			std::cout << "VID\tPID\n";
			for (const auto& hid : hwHid::getAll())
			{
				std::cout << hid.vendor_id << "\t" << hid.product_id << "\t" << hid.getProductName() << "\n";
				std::cout << "\t\t- Usage: " << hid.usage<< "\n";
				std::cout << "\t\t- Usage Page: " << hid.usage_page << "\n";
#if !SOUP_WINDOWS
				std::cout << "\t\t- Have Permission? " << (hid.havePermission() ? "Yes" : "No") << "\n";
#endif
				if (auto serial = hid.getSerialNumber(); !serial.empty())
				{
					std::cout << "\t\t- Serial: " << serial << "\n";
				}
			}
			return 0;
		}

		if (subcommand == "http")
		{
			if (argc != 3)
			{
				std::cout << "Syntax: soup http [uri]" << std::endl;
				return 0;
			}
			auto hr = HttpRequest(Uri(argv[2]));
			auto res = hr.execute();
			if (res.has_value())
			{
				std::cout << res->status_code << "\n";
				std::cout << res->toString() << "\n";
			}
			else
			{
				std::cout << "Request failed.\n";
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

		if (subcommand == "ircserver")
		{
			return cli_ircserver();
		}

		if (subcommand == "keyboard")
		{
			cli_keyboard();
			return 0;
		}

		if (subcommand == "maze")
		{
			cli_maze();
			return 0;
		}

		if (subcommand == "mesh")
		{
			return cli_mesh(argc - 2, &argv[2]);
		}

		if (subcommand == "morse")
		{
			cli_morse(argc - 2, &argv[2]);
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

		if (subcommand == "wav")
		{
#if SOUP_WINDOWS
			if (argc != 3)
			{
				std::cout << "Syntax: soup wav [file]" << std::endl;
				return 0;
			}
			static FileReader fr(argv[2]);
			RiffReader rr(fr);
			if (rr.seekChunk("fmt ").isValid())
			{
				WavFmtChunk fmt;
				fmt.read(fr);
				if (fmt.isGoodForAudPlayback()
					&& rr.seekChunk("data").isValid()
					)
				{
					auto dev = audDevice::getDefault();
					std::cout << "WAV is good, playing on " << dev.getName() << "\n";
					auto pb = dev.open(fmt.channels, [](audPlayback& pb, audSample* block)
					{
						if (!fr.hasMore())
						{
							pb.stop();
							return;
						}
						std::string data;
						fr.str(AUD_BLOCK_BYTES, data);
						memcpy(block, data.data(), data.size());
					});
					pb->awaitCompletion();
					return 0;
				}
			}
			std::cout << "WAV is bad.\n";
#else
			std::cout << "Sorry, this only works on Windows (for now).\n";
#endif
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
- chatbot
- chatgpt [token] <model>
- chess <FEN>
- datareflection
- dhcp
- dig [domain] <type=A> <@<doh:>[server]>
- dnsserver [file] <bind-ip>
- dvd
- edit [files...]
- geoip [ip]
- hid
- http [uri]
- inquire
- ircserver
- keyboard
- maze
- mesh [link <token>|list|dns-add-record [ip] [name] [type] [data]]
- morse [key|encode [text]] <--silent>
- qr [contents]
- repl
- script [.cpp file]
- snake
- test
- wav [file]
- websrv [dir]

Legend: [Required] <Optional>)EOC" << std::endl;
	return 0;
}
