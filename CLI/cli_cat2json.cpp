#include "cli.hpp"

#include <iostream>

#include <cat.hpp>
#include <FileReader.hpp>
#include <JsonObject.hpp>
#include <string.hpp>

using namespace soup;

int cli_cat2json(int argc, const char** argv)
{
	if (argc > 2)
	{
		FileReader fr(argv[2]);
		if (auto cr = cat::parse(fr))
		{
			auto jr = soup::make_unique<JsonObject>();
			for (auto& child : cr->children)
			{
				jr->add(std::move(child->name), std::move(child->value));
			}

			std::string name = argv[2];
			if (name.substr(name.length() - 4) == ".txt")
			{
				name = name.substr(0, name.length() - 4);
			}
			if (name.substr(name.length() - 4) == ".cat")
			{
				name = name.substr(0, name.length() - 4);
			}
			string::toFile(name + ".json", jr->encodePretty());
			return 0;
		}
		std::cout << "Failed to parse input file." << std::endl;
	}
	else
	{
		std::cout << "Syntax: soup cat2json <file>" << std::endl;
	}
	return 1;
}
