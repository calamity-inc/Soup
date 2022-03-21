#include <iostream>

#include <editor.hpp>
#include <string.hpp>
#include <unicode.hpp>

int main(int argc, const char** argv)
{
	std::string subcommand;
	if (argc > 1)
	{
		subcommand = argv[1];
		soup::string::toLower(subcommand);
	}
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
	std::cout << "Syntax: soup edit [files...]" << std::endl;
	return 0;
}
